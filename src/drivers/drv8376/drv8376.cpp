
#include "./drv8376.h"


DRV8376::DRV8376(int nCS, SPISettings settings) : nCS(nCS), settings(settings) {
}


DRV8376::~DRV8376() {
}


void DRV8376::init(SPIClass* _spi) {
    spi = _spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
    }
}


void DRV8376::clearFault() {
    DRV8376_FaultClear_Reg reg;
    reg.FLT_CLR = 1;
    writeFaultClear(reg);
}


void DRV8376::setRegistersLocked(bool locked) {
    DRV8376_SystemCtrl_Reg reg = readSystemCtrl();
    reg.REG_LOCK = locked ? 1 : 0;
    reg.WRITE_KEY = DRV8376_SYSTEM_CTRL_WRITE_KEY;
    writeSystemCtrl(reg);
}


DRV8376_Status_Reg DRV8376::readStatus() {
    DRV8376_Status_Reg reg;
    reg.reg = readRegister(DRV8376_REG_STATUS);
    return reg;
}


DRV8376_RawStatus_Reg DRV8376::readRawStatus() {
    DRV8376_RawStatus_Reg reg;
    reg.reg = readRegister(DRV8376_REG_RAWSTATUS);
    return reg;
}


DRV8376_TempStatus_Reg DRV8376::readTempStatus() {
    DRV8376_TempStatus_Reg reg;
    reg.reg = readRegister(DRV8376_REG_TEMPSTATUS);
    return reg;
}


DRV8376_SupplyStatus_Reg DRV8376::readSupplyStatus() {
    DRV8376_SupplyStatus_Reg reg;
    reg.reg = readRegister(DRV8376_REG_SUPPLYSTATUS);
    return reg;
}


DRV8376_DriverStatus_Reg DRV8376::readDriverStatus() {
    DRV8376_DriverStatus_Reg reg;
    reg.reg = readRegister(DRV8376_REG_DRIVERSTATUS);
    return reg;
}


DRV8376_IntfStatus_Reg DRV8376::readIntfStatus() {
    DRV8376_IntfStatus_Reg reg;
    reg.reg = readRegister(DRV8376_REG_INTFSTATUS);
    return reg;
}


DRV8376_FaultMode_Reg DRV8376::readFaultMode() {
    DRV8376_FaultMode_Reg reg;
    reg.reg = readRegister(DRV8376_REG_FAULT_MODE);
    return reg;
}


void DRV8376::writeFaultMode(const DRV8376_FaultMode_Reg& reg) {
    writeRegister(DRV8376_REG_FAULT_MODE, reg.reg);
}


DRV8376_DriverFaultCtrl_Reg DRV8376::readDriverFaultCtrl() {
    DRV8376_DriverFaultCtrl_Reg reg;
    reg.reg = readRegister(DRV8376_REG_DRIVER_FAULT_CTRL);
    return reg;
}


void DRV8376::writeDriverFaultCtrl(const DRV8376_DriverFaultCtrl_Reg& reg) {
    writeRegister(DRV8376_REG_DRIVER_FAULT_CTRL, reg.reg);
}


DRV8376_FaultClear_Reg DRV8376::readFaultClear() {
    DRV8376_FaultClear_Reg reg;
    reg.reg = readRegister(DRV8376_REG_FAULT_CLEAR);
    return reg;
}


void DRV8376::writeFaultClear(const DRV8376_FaultClear_Reg& reg) {
    writeRegister(DRV8376_REG_FAULT_CLEAR, reg.reg);
}


DRV8376_PWMCtrl1_Reg DRV8376::readPWMCtrl1() {
    DRV8376_PWMCtrl1_Reg reg;
    reg.reg = readRegister(DRV8376_REG_PWM_CTRL_1);
    return reg;
}


void DRV8376::writePWMCtrl1(const DRV8376_PWMCtrl1_Reg& reg) {
    writeRegister(DRV8376_REG_PWM_CTRL_1, reg.reg);
}


DRV8376_PreDriverCtrl_Reg DRV8376::readPreDriverCtrl() {
    DRV8376_PreDriverCtrl_Reg reg;
    reg.reg = readRegister(DRV8376_REG_PREDRIVER_CTRL);
    return reg;
}


void DRV8376::writePreDriverCtrl(const DRV8376_PreDriverCtrl_Reg& reg) {
    writeRegister(DRV8376_REG_PREDRIVER_CTRL, reg.reg);
}


DRV8376_CSAControl_Reg DRV8376::readCSAControl() {
    DRV8376_CSAControl_Reg reg;
    reg.reg = readRegister(DRV8376_REG_CSA_CTRL);
    return reg;
}


void DRV8376::writeCSAControl(const DRV8376_CSAControl_Reg& reg) {
    writeRegister(DRV8376_REG_CSA_CTRL, reg.reg);
}


DRV8376_SystemCtrl_Reg DRV8376::readSystemCtrl() {
    DRV8376_SystemCtrl_Reg reg;
    reg.reg = readRegister(DRV8376_REG_SYSTEM_CTRL);
    return reg;
}


void DRV8376::writeSystemCtrl(const DRV8376_SystemCtrl_Reg& reg) {
    writeRegister(DRV8376_REG_SYSTEM_CTRL, reg.reg);
}


uint16_t DRV8376::readRegister(uint8_t reg) {
    uint8_t head = (reg & 0x3F)<<1 | 0x01; // Read command
    // TODO parity
    uint8_t data[3] = {head, 0, 0};
    transact24(data);
    return (data[1] << 8) | data[2];
}


void DRV8376::writeRegister(uint8_t reg, uint16_t value) {
    uint8_t head = (reg & 0x3F)<<1 | 0x00; // Write command
    // TODO parity
    uint8_t data[3] = {head , (uint8_t)((value >> 8) & 0xFF), (uint8_t)(value & 0xFF)};
    transact24(data);
    // TODO check response
}


void DRV8376::transact24(uint8_t* data) {
    if (nCS >= 0) {
        digitalWrite(nCS, LOW);
    }
    spi->beginTransaction(settings);
    spi->transfer(data, 3);
    spi->endTransaction();
    if (nCS >= 0) {
        digitalWrite(nCS, HIGH);
    }
}



bool DRV8376::getParity(uint16_t data) {
    bool parity = false; // TODO check me
    for (int i = 0; i < 16; i++) {
        parity ^= (data >> i) & 0x01;
    }
    return parity;
}   




void DRV8376Driver3PWM::init(SPIClass* _spi) {
	DRV8376::init(_spi);
	setRegistersLocked(false);
	delayMicroseconds(1);
	DRV8376_PWMCtrl1_Reg pwmReg = readPWMCtrl1();
    pwmReg.PWM_MODE = 0x02; // set 3-PWM mode TODO difference between modes 0x02 and 0x03?
	writePWMCtrl1(pwmReg);
	BLDCDriver3PWM::init();
};




void DRV8376Driver6PWM::init(SPIClass* _spi) {
	DRV8376::init(_spi);
	setRegistersLocked(false);
	delayMicroseconds(1);
	DRV8376_PWMCtrl1_Reg pwmReg = readPWMCtrl1();
    pwmReg.PWM_MODE = 0x00; // set 6-PWM mode TODO difference between modes 0x00 and 0x01?
	writePWMCtrl1(pwmReg);
	BLDCDriver6PWM::init();
};





