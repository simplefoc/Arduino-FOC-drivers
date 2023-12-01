
#include "./STSPIN32G4.h"

#ifdef ARDUINO_GENERIC_G431VBTX


STSPIN32G4::STSPIN32G4() : BLDCDriver6PWM(STSPIN32G4_PIN_INUH, STSPIN32G4_PIN_INUL, STSPIN32G4_PIN_INVH, 
                                          STSPIN32G4_PIN_INVL, STSPIN32G4_PIN_INWH, STSPIN32G4_PIN_INWL),
                           _wire(STSPIN32G4_PIN_SDA, STSPIN32G4_PIN_SCL) {

};



STSPIN32G4::~STSPIN32G4(){
    _wire.end();
};



int STSPIN32G4::init(){
    // init pins
    pinMode(STSPIN32G4_PIN_WAKE, OUTPUT);
    digitalWrite(STSPIN32G4_PIN_WAKE, LOW);
    pinMode(STSPIN32G4_PIN_READY, INPUT_PULLUP);
    pinMode(STSPIN32G4_PIN_FAULT, INPUT_PULLUP);

    int result = this->BLDCDriver6PWM::init();
    if(result == 0) return result;
    setPwm(0,0,0); // set the phases to off

    // init I2C
    _wire.begin();

    delayMicroseconds(50);  // give driver a moment to wake up
    clearFaults();          // clear the faults

    // TODO init fault monitor

    return isReady() ? 1 : 0;
};



void STSPIN32G4::wake() {
    digitalWrite(STSPIN32G4_PIN_WAKE, HIGH);
    delayMicroseconds(50); // 50ms high pulse to wake up
    digitalWrite(STSPIN32G4_PIN_WAKE, LOW);
};



void STSPIN32G4::sleep() {
    digitalWrite(STSPIN32G4_PIN_WAKE, LOW);
    writeRegister(STSPIN32G4_REG_STBY, 0x01);
};



bool STSPIN32G4::isReady(){
    return digitalRead(STSPIN32G4_PIN_READY)==HIGH;
};



bool STSPIN32G4::isFault(){
    return digitalRead(STSPIN32G4_PIN_FAULT)==LOW;
};



STSPIN32G4Status STSPIN32G4::status(){
    STSPIN32G4Status result;
    result.reg = readRegister(STSPIN32G4_REG_STATUS);
    return result;
};



void STSPIN32G4::lock(){
    writeRegister(STSPIN32G4_REG_LOCK, 0x00);
};



void STSPIN32G4::unlock(){
    writeRegister(STSPIN32G4_REG_LOCK, 0xF0);
};



STSPIN32G4NFault STSPIN32G4::getNFaultRegister(){
    STSPIN32G4NFault result;
    result.reg = readRegister(STSPIN32G4_REG_NFAULT);
    return result;
};



STSPIN32G4Ready STSPIN32G4::getReadyRegister(){
    STSPIN32G4Ready result;
    result.reg = readRegister(STSPIN32G4_REG_READY);
    return result;
};



STSPIN32G4Logic STSPIN32G4::getLogicRegister(){
    STSPIN32G4Logic result;
    result.reg = readRegister(STSPIN32G4_REG_LOGIC);
    return result;
};



STSPIN32G4PowMng STSPIN32G4::getPowMngRegister(){
    STSPIN32G4PowMng result;
    result.reg = readRegister(STSPIN32G4_REG_POWMNG);
    return result;
};



void STSPIN32G4::setNFaultRegister(STSPIN32G4NFault value){
    writeRegister(STSPIN32G4_REG_NFAULT, value.reg);
};



void STSPIN32G4::setReadyRegister(STSPIN32G4Ready value){
    writeRegister(STSPIN32G4_REG_READY, value.reg);
};



void STSPIN32G4::setLogicRegister(STSPIN32G4Logic value){
    writeRegister(STSPIN32G4_REG_LOGIC, value.reg);
};



void STSPIN32G4::setPowMngRegister(STSPIN32G4PowMng value){
    writeRegister(STSPIN32G4_REG_POWMNG, value.reg);
};  



void STSPIN32G4::resetRegisters(){
    // write 0xFF to reset register
    writeRegister(STSPIN32G4_REG_RESET, 0xFF);
};



void STSPIN32G4::clearFaults(){
    // write 0xFF to clear faults
    writeRegister(STSPIN32G4_REG_CLEAR, 0xFF);
};



uint8_t STSPIN32G4::readRegister(uint8_t reg){
    uint8_t result = 0;
    _wire.beginTransmission(STSPIN32G4_I2C_ADDR);
    _wire.write(reg);
    _wire.endTransmission(false);
    _wire.requestFrom(STSPIN32G4_I2C_ADDR, 1);
    result = _wire.read();
    return result;
};



void STSPIN32G4::writeRegister(uint8_t reg, uint8_t val){
    _wire.beginTransmission(STSPIN32G4_I2C_ADDR);
    _wire.write(reg);
    _wire.write(val);
    _wire.endTransmission();
};


#endif