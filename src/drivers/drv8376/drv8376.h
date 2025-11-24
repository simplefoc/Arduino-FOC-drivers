
#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <SimpleFOC.h>

#include "./drv8376_registers.h"


/*
Data is captured on the falling edge of the SCLK pin and data is propagated on the rising edge of the SCLK
pin. The most significant bit (MSB) is shifted in and out first.
*/
#define DRV8376SPISettings SPISettings(1000000, MSBFIRST, SPI_MODE3)


class DRV8376 {
public:
    DRV8376(int nCS = -1, SPISettings settings = DRV8376SPISettings);
    virtual ~DRV8376();

    virtual void init(SPIClass* _spi = &SPI);

    void clearFault();

    void setRegistersLocked(bool locked);

    DRV8376_Status_Reg readStatus();
    DRV8376_RawStatus_Reg readRawStatus();
    DRV8376_TempStatus_Reg readTempStatus();
    DRV8376_SupplyStatus_Reg readSupplyStatus();
    DRV8376_DriverStatus_Reg readDriverStatus();
    DRV8376_IntfStatus_Reg readIntfStatus();

    DRV8376_FaultMode_Reg readFaultMode();
    void writeFaultMode(const DRV8376_FaultMode_Reg& reg);
    DRV8376_DriverFaultCtrl_Reg readDriverFaultCtrl();
    void writeDriverFaultCtrl(const DRV8376_DriverFaultCtrl_Reg& reg);
    DRV8376_FaultClear_Reg readFaultClear();
    void writeFaultClear(const DRV8376_FaultClear_Reg& reg);
    DRV8376_PWMCtrl1_Reg readPWMCtrl1();
    void writePWMCtrl1(const DRV8376_PWMCtrl1_Reg& reg);
    DRV8376_PreDriverCtrl_Reg readPreDriverCtrl();
    void writePreDriverCtrl(const DRV8376_PreDriverCtrl_Reg& reg);
    DRV8376_CSAControl_Reg readCSAControl();
    void writeCSAControl(const DRV8376_CSAControl_Reg& reg);
    DRV8376_SystemCtrl_Reg readSystemCtrl();
    void writeSystemCtrl(const DRV8376_SystemCtrl_Reg& reg);

protected:
    SPIClass* spi;
    SPISettings settings;
    int nCS;

    uint16_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint16_t value);
    void transact24(uint8_t* data);
    bool getParity(uint16_t data);
};






class DRV8376Driver3PWM : public DRV8376, public BLDCDriver3PWM {

	public:
		DRV8376Driver3PWM(int phA,int phB,int phC, int cs = -1, int en = NOT_SET, SPISettings settings = DRV8376SPISettings) :
			DRV8376(cs, settings), BLDCDriver3PWM(phA, phB, phC, en) { enable_active_high=false; };
		virtual ~DRV8376Driver3PWM() {};

		virtual void init(SPIClass* _spi = &SPI) override;

};



class DRV8376Driver6PWM : public DRV8376, public BLDCDriver6PWM {

	public:
		DRV8376Driver6PWM(int phA_h,int phA_l,int phB_h,int phB_l,int phC_h,int phC_l, int cs = -1, int en = NOT_SET, SPISettings settings = DRV8376SPISettings) :
			DRV8376(cs, settings), BLDCDriver6PWM(phA_h, phA_l, phB_h, phB_l, phC_h, phC_l, en) { enable_active_high=false; };
		virtual ~DRV8376Driver6PWM() {};

		virtual void init(SPIClass* _spi = &SPI) override;

};




