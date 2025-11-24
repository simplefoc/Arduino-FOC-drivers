#pragma once

#include <inttypes.h>


#define DRV8376_REG_STATUS       0x00
#define DRV8376_REG_RAWSTATUS    0x02
#define DRV8376_REG_TEMPSTATUS   0x04
#define DRV8376_REG_SUPPLYSTATUS 0x05
#define DRV8376_REG_DRIVERSTATUS 0x06
#define DRV8376_REG_INTFSTATUS   0x07


#define DRV8376_REG_FAULT_MODE        0x10
#define DRV8376_REG_DRIVER_FAULT_CTRL 0x13
#define DRV8376_REG_FAULT_CLEAR       0x17
#define DRV8376_REG_PWM_CTRL_1        0x20
#define DRV8376_REG_PREDRIVER_CTRL    0x22
#define DRV8376_REG_CSA_CTRL          0x23
#define DRV8376_REG_SYSTEM_CTRL       0x3F



#define DRV8376_SYSTEM_CTRL_WRITE_KEY   0x05


/**
 * STATUS REGISTERS
 */


typedef union {
    struct {
        uint16_t FAULT : 1;         // 0h = No fault condition is detected
                                    // 1h = Fault condition is detected
        uint16_t OTF : 1;           // 0h = No overtemperature warning / shutdown is detected
                                    // 1h = Overtemperature warning / shutdown is detected
        uint16_t UVP : 1;           // 0h = No undervoltage voltage condition is detected on CP
                                    // 1h = Undervoltage voltage condition is detected on CP
        uint16_t OVP : 1;           // 0h = No over voltage condition is detected
                                    // 1h = Over voltage condition is detected
        uint16_t : 1;
        uint16_t OCP : 1;           // 0h = No overcurrent condition is detected
                                    // 1h = Overcurrent condition is detected
        uint16_t SPIFLT : 1;        // 0h = No SPI fault is detected
                                    // 1h = SPI fault is detected
        uint16_t RESET : 1;         // 0h = Cleared Value
                                    // 1h = Device has undergone power on reset
        uint16_t SYSFLT : 1;        // 0h = No OTP read fault is detected
                                    // 1h = OTP read fault detected
        uint16_t DNRDY_STS : 1;     // 0h = Device is Ready
                                    // 1h = Device is NOT Ready
        uint16_t : 5;
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_Status_Reg;





typedef union {
    struct {
        uint16_t : 1;
        uint16_t OTF_RSTS : 1;      // Overtemperature Shutdown Raw Fault Status
        uint16_t UVP_RSTS : 1;      // CP Undervoltage Raw Fault Status.
        uint16_t OVP_RSTS : 1;      // Overvoltage Raw Fault Status
        uint16_t : 1;
        uint16_t OCP_RSTS : 1;      // Overcurrent Fault Raw Status
        uint16_t SPIFLT_RSTS : 1;   // SPI Fault status. Status remains latched until cleared by write to FLT_CLR or reset pulse on nSLEEP
        uint16_t RESET : 1;
        uint16_t SYSFLT_RSTS : 1;   // OTP Read fault occurred. Status remains latched until cleared by write to FLT_CLR
        uint16_t DNRDY_RSTS : 1;    // Device Not Ready Status
        uint16_t : 1;
        uint16_t OTW_RSTS : 1;      // OT Warning Raw Status
        uint16_t DRVOFF_RSTS : 1;   // Status of DRV_OFF pin
        uint16_t : 2;
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_RawStatus_Reg;






typedef union {
    struct {
        uint16_t OTSD : 1;          // Overtemperature Shutdown Fault status. Can be cleared by write to
                                    // FLT_CLR or reset pulse on nSLEEP
                                    // 0h = No overtemperature shutdown is detected
                                    // 1h = Overtemperature shutdown is detected
        uint16_t OTW : 1;           // Overtemperature Warning Fault status. Can be cleared by write to
                                    // FLT_CLR or reset pulse on nSLEEP
                                    // 0h = No overtemperature warning is detected
                                    // 1h = Overtemperature warning is detected
        uint16_t : 13;
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_TempStatus_Reg;






typedef union {
    struct {
        uint16_t : 4;
        uint16_t CP_UV : 1;     // Charge Pump Undervoltage fault status
                                // 0h = No charge pump undervoltage is detected
                                // 1h = Charge pump undervoltage is detected
        uint16_t : 1;
        uint16_t VM_OV : 1;     // Vm Over Voltage Fault Status
                                // 0h = No Vm over voltage is detected
                                // 1h = Vm over voltage is detected
        uint16_t : 8;
        uint16_t PARITY : 1;    // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_SupplyStatus_Reg;





typedef union {
    struct {
        uint16_t OCPA_LS : 1;    // 0h = No overcurrent detected on low-side MOSFET of OUTA
                                 // 1h = Overcurrent detected on low-side MOSFET of OUTA
        uint16_t OCPB_LS : 1;    // 0h = No overcurrent detected on low-side MOSFET of OUTB
                                 // 1h = Overcurrent detected on low-side MOSFET of OUTB
        uint16_t OCPC_LS : 1;    // 0h = No overcurrent detected on low-side MOSFET of OUTC
                                 // 1h = Overcurrent detected on low-side MOSFET of OUTC
        uint16_t : 1;
        uint16_t OCPA_HS : 1;    // 0h = No overcurrent detected on high-side MOSFET of OUTA
                                 // 1h = Overcurrent detected on high-side MOSFET of OUTA
        uint16_t OCPB_HS : 1;    // 0h = No overcurrent detected on high-side MOSFET of OUTB
                                 // 1h = Overcurrent detected on high-side MOSFET of OUTB
        uint16_t OCPC_HS : 1;    // 0h = No overcurrent detected on high-side MOSFET of OUTC
                                 // 1h = Overcurrent detected on high-side MOSFET of OUTC
        uint16_t : 8;
        uint16_t PARITY : 1;     // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_DriverStatus_Reg;





typedef union {
    struct {
        uint16_t FRM_ERR : 1;       // 0h = No SPI Frame Error is detected
                                    // 1h = SPI Frame Error is detected
        uint16_t : 1;
        uint16_t SPI_PARITY : 1;    // 0h = No SPI Parity Error is detected
                                    // 1h = SPI Parity Error is detected
        uint16_t : 1;
        uint16_t OTPLD_ERR : 1;     // 0h = No OTP read error is detected
                                    // 1h = OTP read error is detected
        uint16_t : 10;
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_IntfStatus_Reg;







/**
 * CONTROL REGISTERS
 */


typedef union {
    struct {
        uint16_t OTW_MODE : 1;      // 0h = Over temperature reporting on nFAULT is disabled
                                    // 1h = Over temperature reporting on nFAULT is enabled
        uint16_t : 3;
        uint16_t OCP_MODE : 2;      // 0h = Over Current causes a latched fault
                                    // 1h = Over Current causes an automatic retrying fault
                                    // 2h = Over Current is report only but no action is taken
                                    // 3h = Over Current is not reported and no action is taken
        uint16_t : 1;
        uint16_t SPIFLT_MODE : 1;   // 0h = SPI fault reporting on nFAULT pin is disabled
                                    // 1h = SPI fault reporting on nFAULT pin is enabled
        uint16_t : 1;
        uint16_t OVP_MODE : 1;      // 0h = Over Voltage protection is disabled
                                    // 1h = Over Voltage protection is enabled
        uint16_t : 3;
        uint16_t ILIMFLT_MODE : 1;  // 0h = ILIMIT reporting on nFAULT pin is disabled
                                    // 1h = ILIMIT reporting on nFAULT pin is enabled
        uint16_t : 1;
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_FaultMode_Reg;





typedef union {
    struct {
        uint16_t OCP_LVL : 1;     // 0h = 4.5A
                                  // 1h = 2.5A
        uint16_t : 1;
        uint16_t OCP_TRETRY : 1;  // 0h = 5ms
                                  // 1h = 500ms
        uint16_t : 1;
        uint16_t OCP_DEG : 2;     // 0h = OCP Deglitch time is 0.6 µs
                                  // 1h = OCP Deglitch time is 1.25 µs
                                  // 2h = OCP Deglitch time is 1.6 µs
                                  // 3h = OCP Deglitch time is 2 µs
        uint16_t : 2;
        uint16_t OVP_SEL : 1;     // 0h = VM overvoltage level is 65V
                                  // 1h = VM overvoltage level is 35V
        uint16_t : 6;
        uint16_t PARITY : 1;      // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_DriverFaultCtrl_Reg;





typedef union {
    struct {
        uint16_t FLT_CLR : 1;       // 0h = No clear fault command is issued
                                    // 1h = To clear the latched faults, write '1' to this bit
        uint16_t : 14;
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_FaultClear_Reg;


typedef union {
    struct {
        uint16_t PWM_MODE : 2;          // 0h = 6x mode
                                        // 1h = 6x mode
                                        // 2h = 3x mode
                                        // 3h = 3x mode
        uint16_t EN_ASR : 1;            // 0h = Active Demagnetization is Disabled
                                        // 1h = Active Demagnetization is Enabled
        uint16_t EN_AAR : 1;            // 0h = Active Demagnetization AAR is Disabled
                                        // 1h = Active Demagnetization AAR is Enabled
        uint16_t : 1;
        uint16_t ILIM_MODE : 1;         // 0h = Current recirculation through FETs (Brake mode)
                                        // 1h = Current recirculation through diodes (coast mode)
        uint16_t PWM_100_FREQ_SEL : 2;  // 0h = 20KHz
                                        // 1h = 40KHz
                                        // 2h = 10KHz
                                        // 3h = None
        uint16_t : 7;
        uint16_t PARITY : 1;            // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_PWMCtrl1_Reg;





typedef union {
    struct {
        uint16_t SLEW_RATE : 2;         // Slew rate settings
                                        // 0h = Slew rate is 1100 V/µs
                                        // 1h = Slew rate is 500 V/µs
                                        // 2h = Slew rate is 250 V/µs
                                        // 3h = Slew rate is 50 V/µs
        uint16_t AD_COMP_TH_LS : 1;     // Active demag low side comparator threshold
                                        // 0h = active demag comparator threshold is 100mA
                                        // 1h = active demag comparator threshold is 150mA
        uint16_t AD_COMP_TH_HS : 1;     // Active demag high side comparator threshold
                                        // 0h = active demag comparator threshold is 100mA
                                        // 1h = active demag comparator threshold is 150mA
        uint16_t ADMAG_TMARGIN : 4;     // Wait time before determining HiZ. N*4*100ns
        uint16_t ILIM_BLANK_SEL : 3;    // Current Limit Blanking Time Selection
                                        // 0h = 5.5us for slew rate of 50 and 1.8us for all other slew rates.
                                        // 1h = 6.0us for slew rate of 50 and 2.3us for all other slew rates.
                                        // 2h = 6.5us for slew rate of 50 and 2.8us for all other slew rates.
                                        // 3h = 7.5us for slew rate of 50 and 3.8us for all other slew rates.
        uint16_t : 4;
        uint16_t PARITY : 1;            // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_PreDriverCtrl_Reg;





typedef union {
    struct {
        uint16_t CSA_GAIN : 2;      // 0h = CSA gain is 0.4 V/A
                                    // 1h = CSA gain is 1.0 V/A
                                    // 2h = CSA gain is 2.5 V/A
                                    // 3h = CSA gain is 5.0 V/A
        uint16_t : 13;
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_CSAControl_Reg;





typedef union {
    struct {
        uint16_t : 6;
        uint16_t SPI_PEN : 1;       // Parity Enable for SPI
                                    // 0h = Parity Disabled
                                    // 1h = Parity Enabled
        uint16_t REG_LOCK : 1;      // Register Lock Bit
                                    // 0h = Registers Unlocked
                                    // 1h = Registers Locked
        uint16_t : 2;
        uint16_t SDO_ODEN : 1;      // SDO in Open Drain Mode
                                    // 0h = SDO in Push Pull Mode
                                    // 1h = SDO in Open Drain Mode
        uint16_t SDO_VSEL : 1;      // SDO Output Voltage Select
                                    // 0h = AVDD
                                    // 1h = GVDD
        uint16_t WRITE_KEY : 3;     // 0x5 Write Key Specific to this register.
        uint16_t PARITY : 1;        // 15 Parity Bit if SPI_PEN is set to '1' otherwise reserved
    };
    uint16_t reg;
} DRV8376_SystemCtrl_Reg;

