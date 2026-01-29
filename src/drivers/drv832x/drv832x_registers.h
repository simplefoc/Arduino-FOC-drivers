

#pragma once


#define Fault_Status_ADDR   0x0
#define VGS_Status_ADDR   0x1
#define Driver_Control_ADDR   0x2
#define Gate_Drive_HS_ADDR  0x3
#define Gate_Drive_LS_ADDR  0x4
#define OCP_Control_ADDR  0x5
#define CSA_Control_ADDR  0x6


//Read-only
typedef union {
	struct 
	{

		bool VDS_LC:1, //Overcurrent on C low-side mosfet
		VDS_HC:1, //Overcurrent on C high-side mosfet
		VDS_LB:1, //Overcurrent on B low-side mosfet
		VDS_HB:1, //Overcurrent on B high-side mosfet
		VDS_LA:1, //Overcurrent on A low-side mosfet
		VDS_HA:1, //Overcurrent on A high-side mosfet
		OTSD:1, //Overtemperature shutdown fault
		UVLO:1, //undervoltage lockout fault
		GDF:1, //Gate drive fault
		VDS_OCP:1, //VDS monitor overcurrent fault
		FAULT:1; //Mirrors nFAULT pin
		//uint8_t :5;
	};
	uint16_t reg : 11;
} Fault;

//Read-only
typedef union {
	struct 
	{

		bool VGS_LC:1, //Gate drive fault on C low-side mosfet
		VGS_HC:1, //Gate drive fault on C high-side mosfet
		VGS_LB:1, //Gate drive fault on B low-side mosfet
		VGS_HB:1, //Gate drive fault on B high-side mosfet
		VGS_LA:1, //Gate drive fault on A low-side mosfet
		VGS_HA:1, //Gate drive fault on A high-side mosfet
		CPUV:1, //Charge-pump undervoltage fault
		OTW:1, //Overtemp warning
		SC_OC:1, //Overcurrent on phase C sense amplifier
		SB_OC:1, //Overcurrent on phase B sense amplifier
		SA_OC:1; //Overcurrent on phase A sense amplifier
		//uint8_t :5;
	};
	uint16_t reg : 11;
} VGS;

typedef union {
	struct 
	{

		bool CLR_FLT:1, //Clear faults, automatically clears itself
		BRAKE:1, //Turn on all low-side mosfets in 1PWM
		COAST:1, //Put mosfet in Hi-Z
		_1PWM_DIR:1, //Invert 1PWM dir pin
		_1PWM_COM:1; //1PWM asynchronous rectification
		uint8_t PWM_MODE:2; //00 = 6PWM, 01=3PWM, 10=1PWM, 11=Independant
		bool OTW_REP:1, //Overtemp warning report on nFAULT
		DIS_GDF:1, //Disable gate drive fault
		DIS_CPUV:1, //Disable charge pump undervoltage lockout fault
		:1;
		//uint8_t :5;
	};
	uint16_t reg : 11;
} Driver_Control;

typedef union {
	struct 
	{
		uint8_t IDRIVEN_HS:4, //high side discharge current
		IDRIVEP_HS:4, //high side charge current
		LOCK:3; //110 = lock registers, 011b = unlock registers
		//uint8_t :5;
	};
	uint16_t reg : 11;
} Gate_Drive_HS;

typedef union {
	struct 
	{
		uint8_t IDRIVEN_LS:4, //low side discharge current
		IDRIVEP_LS:4, //low side charge current
		TDRIVE:2; //gate current peak duration
		bool CBC:1; //cycle-by-cycle operation
		//uint8_t :5;
	};
	uint16_t reg : 11;
} Gate_Drive_LS;

typedef union {
	struct 
	{
		uint8_t VDS_LVL:4, //vds protection voltage
		OCP_DEG:2, //overcurrent deglitch time
		OCP_MODE:2, //overcurrent fault registering
		DEAD_TIME:2; //dead-time
		bool TRETRY:1; //VDS_OCP and SEN_OCP retry time
		//uint8_t :5;
	};
	uint16_t reg : 11;
} OCP_Control;

//Only for DRV8323
typedef union {
	struct 
	{
		uint8_t SEN_LVL:2; //Sense OCP voltage (value*0.25) V
		bool CSA_CAL_C:1, //Short inputs of current sense for offset cal
		CSA_CAL_B:1, //Short inputs of current sense for offset cal
		CSA_CAL_A:1, //Short inputs of current sense for offset cal
		DIS_SEN:1; //Disable sense overcurrent fault
		uint8_t CSA_GAIN:2; //(2^value)*5 V/V sensitivity
		bool LS_REF:1, //VDS OCP measurement source
		VREF_DIV:1, //Unidirectional current sense
		CSA_FET:1; //Change current sense amp positive input
		//uint8_t :5;
	};
	uint16_t reg : 11;
} CSA_Control;