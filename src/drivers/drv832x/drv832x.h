

#ifndef SIMPLEFOC_DRV832x
#define SIMPLEFOC_DRV832x


#include "Arduino.h"
#include <SPI.h>
#include <drivers/BLDCDriver3PWM.h>
#include <drivers/BLDCDriver6PWM.h>

#include "./drv832x_registers.h"


enum DRV832x_PWMMode {
	PWM6_Mode = 0b00, //default
	PWM3_Mode = 0b01,
	PWM1_Mode = 0b10,
	Independant_Mode = 0b11
};

enum DRV832x_IDRIVEP {
	IDRIVEP_10mA,
	IDRIVEP_30mA,
	IDRIVEP_60mA,
	IDRIVEP_80mA,
	IDRIVEP_120mA,
	IDRIVEP_140mA,
	IDRIVEP_170mA,
	IDRIVEP_190mA,
	IDRIVEP_260mA,
	IDRIVEP_330mA,
	IDRIVEP_370mA,
	IDRIVEP_440mA,
	IDRIVEP_570mA,
	IDRIVEP_680mA,
	IDRIVEP_820mA,
	IDRIVEP_1000mA, //default
};

enum DRV832x_IDRIVEN {
	IDRIVEN_20mA,
	IDRIVEN_60mA,
	IDRIVEN_120mA,
	IDRIVEN_160mA,
	IDRIVEN_240mA,
	IDRIVEN_280mA,
	IDRIVEN_340mA,
	IDRIVEN_380mA,
	IDRIVEN_520mA,
	IDRIVEN_660mA,
	IDRIVEN_740mA,
	IDRIVEN_880mA,
	IDRIVEN_1140mA,
	IDRIVEN_1360mA,
	IDRIVEN_1640mA,
	IDRIVEN_2000mA, //default
};

enum DRV832x_LOCK
{
	LOCK_LOCK=0b110,
	LOCK_UNLOCK=0b011 //default
};

enum DRV832x_TDRIVE
{
	TDRIVE_500ns,
	TDRIVE_1000ns,
	TDRIVE_2000ns,
	TDRIVE_4000ns //default
};

enum DRV832x_DEADTIME
{
	DEADTIME_50ns,
	DEADTIME_100ns, //default
	DEADTIME_200ns,
	DEADTIME_400ns
};

enum DRV832x_OCPMode
{
	OCPMode_LATCH_FAULT,
	OCPMode_RETRY_FAULT, //default
	OCPMode_REPORT,
	OCPMode_IGNORE
};

enum DRV832x_OCPDeglitch
{
	OCPDeglitch_2us,
	OCPDeglitch_4us,//default
	OCPDeglitch_6us,
	OCPDeglitch_8us
};

enum DRV832x_VDS_LVL
{
	VDS_LVL_60mV,
	VDS_LVL_130mV,
	VDS_LVL_200mV,
	VDS_LVL_260mV,
	VDS_LVL_310mV,
	VDS_LVL_450mV,
	VDS_LVL_530mV,
	VDS_LVL_600mV,
	VDS_LVL_680mV,
	VDS_LVL_750mV, //default
	VDS_LVL_940mV,
	VDS_LVL_1130mV,
	VDS_LVL_1300mV,
	VDS_LVL_1500mV,
	VDS_LVL_1700mV,
	VDS_LVL_1880mV,
};

enum DRV832x_CSAGain
{
	CSAGain_5,
	CSAGain_10,
	CSAGain_20,//default
	CSAGain_40
};

enum DRV832x_CS_VSEN_LVL
{
	CSAGain_250mV,
	CSAGain_500mV,
	CSAGain_750mV,
	CSAGain_1000mV//default
};

class DRV832xFault {
public:
	DRV832xFault(Fault status) : fault_reg(status) {};
	~DRV832xFault() {};

	bool isFault() { return fault_reg.FAULT; };
	bool isOverCurrent() { return fault_reg.VDS_OCP; };
	bool isGateDriveFault() { return fault_reg.GDF; };
	bool isUndervoltageLockout() { return fault_reg.UVLO; };
	bool isOverTemperature() { return fault_reg.OTSD; };
	bool isOverCurrent_Ah() {return fault_reg.VDS_HA; };
	bool isOverCurrent_Al() {return fault_reg.VDS_LA; };
	bool isOverCurrent_Bh() {return fault_reg.VDS_HB; };
	bool isOverCurrent_Bl() {return fault_reg.VDS_LB; };
	bool isOverCurrent_Ch() {return fault_reg.VDS_HC; };
	bool isOverCurrent_Cl() {return fault_reg.VDS_LC; };

	Fault fault_reg;
};


class DRV832xVGS {
public:
	DRV832xVGS(VGS status) : vgs_reg(status) {};
	~DRV832xVGS() {};


	bool isOverCurrent_As() { return vgs_reg.SA_OC==0b1; };
	bool isOverCurrent_Bs() { return vgs_reg.SB_OC==0b1; };
	bool isOverCurrent_Cs() { return vgs_reg.SC_OC==0b1; };
	bool isOverTemperatureWarning() { return vgs_reg.OTW==0b1; };
	bool isChargePumpUndervoltage() { return vgs_reg.CPUV==0b1; };
	bool isGateDriveFault_Ah() { return vgs_reg.VGS_HA==0b1; };
	bool isGateDriveFault_Al() { return vgs_reg.VGS_LA==0b1; };
	bool isGateDriveFault_Bh() { return vgs_reg.VGS_HB==0b1; };
	bool isGateDriveFault_Bl() { return vgs_reg.VGS_LB==0b1; };
	bool isGateDriveFault_Ch() { return vgs_reg.VGS_HC==0b1; };
	bool isGateDriveFault_Cl() { return vgs_reg.VGS_LC==0b1; };

	VGS vgs_reg;
};



class DRV832xStatus : public DRV832xFault, public DRV832xVGS {
	public:
		DRV832xStatus(Fault faults, VGS vgs) : DRV832xFault(faults), DRV832xVGS(vgs) {};
		~DRV832xStatus() {};
};

#define decl_getset(name, type) type get##name(); void set##name(type);
#define decl_isset(name) bool is##name(); void set##name(bool);

class DRV832xDriver {

	public:
		DRV832xDriver(int cs, bool currentLimit = false, int nFault = NOT_SET) : currentLimit(currentLimit), cs(cs), nFault(nFault), spi(&SPI), settings(1000000, MSBFIRST, SPI_MODE1) {};
		virtual ~DRV832xDriver() {};

		virtual void init(SPIClass* _spi = &SPI);

		void clearFault(); // TODO check for fault condition methods

		DRV832xStatus getStatus();

		decl_isset(RegistersLocked);

		//Driver control
		decl_isset(Brake);
		decl_isset(Coast);
		decl_isset(OnePWMInvertDirection);
		decl_isset(OnePWMAsynchronousRectification);
		decl_getset(PWMMode, DRV832x_PWMMode);
		decl_isset(OvertemperatureReporting);
		decl_isset(DisableGateDriveFault);
		decl_isset(DisableChargePumpUndervoltage);

		//OCP Control
		decl_getset(VDSLevel, DRV832x_VDS_LVL);
		decl_getset(OCPMode, DRV832x_OCPMode);
		decl_getset(OCPDeglitchTime, DRV832x_OCPDeglitch);
		decl_getset(DeadTime, DRV832x_DEADTIME);

		//Drive current
		decl_getset(HighSideChargeCurrent, DRV832x_IDRIVEP);
		decl_getset(HighSideDischargeCurrent, DRV832x_IDRIVEN);
		decl_getset(LowSideChargeCurrent, DRV832x_IDRIVEP);
		decl_getset(LowSideDischargeCurrent, DRV832x_IDRIVEN);

		//Misc drive
		decl_getset(DriveTime, DRV832x_TDRIVE);
		decl_isset(CycleByCycle);
		decl_isset(RetryTime);
		
		//Current sense (DRV8323 only)
		decl_getset(CurrentSenseOvercurrentSensitivity, DRV832x_CS_VSEN_LVL);
		decl_isset(CurrentSenseCalibrateA);
		decl_isset(CurrentSenseCalibrateB);
		decl_isset(CurrentSenseCalibrateC);
		decl_isset(CurrentSenseOvercurrentDisable);
		decl_getset(CurrentSenseGain, DRV832x_CSAGain);
		decl_isset(CurrentSenseOvercurrentResistor);
		decl_isset(CurrentSenseBidirectionnal);
		decl_isset(CurrentSenseFET);

	private:
		uint16_t readSPI(uint8_t addr);
		uint16_t writeSPI(uint8_t addr, uint16_t value);

		bool currentLimit;
		int cs;
		int nFault;
		SPIClass* spi;
		SPISettings settings;
};

#undef decl_getset
#undef decl_isset

class DRV832xDriver3PWM : public DRV832xDriver, public BLDCDriver3PWM {

	public:
		DRV832xDriver3PWM(int phA,int phB,int phC, int cs, bool currentLimit = false, int en = NOT_SET, int nFault = NOT_SET) :
			DRV832xDriver(cs, currentLimit, nFault), BLDCDriver3PWM(phA, phB, phC, en) { enable_active_high=false; };
		virtual ~DRV832xDriver3PWM() {};

		virtual void init(SPIClass* _spi = &SPI) override;

};



class DRV832xDriver6PWM : public DRV832xDriver, public BLDCDriver6PWM {

	public:
		DRV832xDriver6PWM(int phA_h,int phA_l,int phB_h,int phB_l,int phC_h,int phC_l, int cs, bool currentLimit = false, int en = NOT_SET, int nFault = NOT_SET) :
			DRV832xDriver(cs, currentLimit, nFault), BLDCDriver6PWM(phA_h, phA_l, phB_h, phB_l, phC_h, phC_l, en) { enable_active_high=false; };
		virtual ~DRV832xDriver6PWM() {};

		virtual void init(SPIClass* _spi = &SPI) override;

};


#endif
