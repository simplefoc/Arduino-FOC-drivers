/*
 * AS5047U.cpp
 *
 *  Created on: 24 Feb 2022
 *      Author: runger
 */

#include "./AS5047U.h"

AS5047U::AS5047U(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {
	// nix
}

AS5047U::~AS5047U() {
}


void AS5047U::init(SPIClass* _spi) {
	spi = _spi;
	if (nCS>=0) {
		pinMode(nCS, OUTPUT);
		digitalWrite(nCS, HIGH);
	}
	//SPI has an internal SPI-device counter, it is possible to call "begin()" from different devices
	spi->begin();
	readRawAngle(); // read an angle
}

float AS5047U::getCurrentAngle(){
	readCorrectedAngle();
	return ((float)readCorrectedAngle())/(float)AS5047U_CPR * 2.0f * (float)PI;
}

float AS5047U::getFastAngle(){
	return ((float)readCorrectedAngle())/(float)AS5047U_CPR * 2.0f * (float)PI;
}


uint16_t AS5047U::readRawAngle(){
	uint16_t command = AS5047U_ANGLEUNC_REG | AS5047U_RW;
	uint16_t lastresult = spi_transfer16(command)&AS5047U_RESULT_MASK;
	return lastresult;
}


uint16_t AS5047U::readCorrectedAngle(){
	uint16_t command = AS5047U_ANGLECOM_REG | AS5047U_RW;
	uint16_t lastresult = spi_transfer16(command)&AS5047U_RESULT_MASK;
	return lastresult;
}


uint16_t AS5047U::readMagnitude(){
	uint16_t command = AS5047U_MAGNITUDE_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop16();
	return result;
}


uint16_t AS5047U::readVelocity(){
	uint16_t command = AS5047U_VELOCITY_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop16();
	return result;
}


bool AS5047U::isErrorFlag(){
	return errorflag;
}

bool AS5047U::isWarningFlag(){
	return warningflag;
}


AS5047UError AS5047U::clearErrorFlag(){
	uint16_t command = AS5047U_ERROR_REG | AS5047U_RW; // set r=1, result is 0x4001
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047UError result;
	result.reg = nop16();
	return result;
}


AS5047USettings1 AS5047U::readSettings1(){
	uint16_t command = AS5047U_SETTINGS1_REG | AS5047U_RW; // set r=1, result is 0xC018
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047USettings1 result = {
			.reg = nop16()
	};
	return result;
}




void AS5047U::writeSettings1(AS5047USettings1 settings){
	writeRegister24(AS5047U_SETTINGS1_REG, settings.reg);
}




AS5047USettings2 AS5047U::readSettings2(){
	uint16_t command = AS5047U_SETTINGS2_REG | AS5047U_RW; // set r=1, result is 0x4019
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047USettings2 result = {
			.reg = nop16()
	};
	return result;
}





void AS5047U::writeSettings2(AS5047USettings2 settings){
	writeRegister24(AS5047U_SETTINGS2_REG, settings.reg);
}





AS5047USettings3 AS5047U::readSettings3(){
	uint16_t command = AS5047U_SETTINGS3_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047USettings3 result = {
			.reg = nop16()
	};
	return result;
}





void AS5047U::writeSettings3(AS5047USettings3 settings){
	writeRegister24(AS5047U_SETTINGS3_REG, settings.reg);
}




AS5047UDiagnostics AS5047U::readDiagnostics(){
	uint16_t command = AS5047U_DIAGNOSTICS_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047UDiagnostics result = {
			.reg = nop16()
	};
	return result;
}




uint8_t AS5047U::readAGC(){
	uint16_t command = AS5047U_AGC_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop16();
	return result & 0x00FF;
};



uint8_t AS5047U::readECCCHK(){
	uint16_t command = AS5047U_ECCCHK_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop16();
	return result & 0x007F;
};




AS5047UDisableSettings AS5047U::readDisableSettings(){
	uint16_t command = AS5047U_DISABLE_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047UDisableSettings result = {
			.reg = nop16()
	};
	return result;
};



void AS5047U::writeDisableSettings(AS5047UDisableSettings settings){
	writeRegister24(AS5047U_DISABLE_REG, settings.reg);
};



AS5047UECCSettings AS5047U::readECCSettings(){
	uint16_t command = AS5047U_ECC_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047UECCSettings result = {
			.reg = nop16()
	};
	return result;
};



void AS5047U::writeECCSettings(AS5047UECCSettings settings){
	writeRegister24(AS5047U_ECC_REG, settings.reg);
};







void AS5047U::enablePWM(bool enable, bool pwmOnWPin){
	// AS5047UDisableSettings settings = readDisableSettings();
	// if (settings.uvw_off==1) {
	// 	settings.uvw_off = 0;
	// 	writeDiableSettings(settings);
	// }
	AS5047USettings2 settings2 = readSettings2();
	settings2.uvw_abi = pwmOnWPin?0:1;
	settings2.pwm_on = enable;
	writeSettings2(settings2);
}




void AS5047U::enableABI(bool enable){
	AS5047UDisableSettings settings = readDisableSettings();
	settings.abi_off = enable?0:1;
	writeDisableSettings(settings);
	delayMicroseconds(50);
	if (enable) {
		AS5047USettings2 settings2 = readSettings2();
		settings2.uvw_abi = 0;
		writeSettings2(settings2);
	}
}



void AS5047U::enableUVW(bool enable){
	AS5047UDisableSettings settings = readDisableSettings();
	settings.uvw_off = enable?0:1;
	writeDisableSettings(settings);
	if (enable) {
		AS5047USettings2 settings2 = readSettings2();
		settings2.uvw_abi = 1;
		writeSettings2(settings2);
	}
}




uint16_t AS5047U::getZero(){
	uint16_t command = AS5047U_ZPOSM_REG | AS5047U_RW;
	spi_transfer16(command);
	command = AS5047U_ZPOSL_REG | AS5047U_RW;
	uint16_t result = spi_transfer16(command);
	AS5047UZPosL posL = {
			.reg = nop16()
	};
	return ((result&0x00FF)<<6) | posL.zposl;
}



uint16_t AS5047U::setZero(uint16_t value){
	uint16_t command = AS5047U_ZPOSL_REG | AS5047U_RW;
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047UZPosL posL = {
			.reg = nop16()
	};
	posL.zposl = value&0x003F;
	writeRegister24(AS5047U_ZPOSL_REG, posL.reg);
	writeRegister24(AS5047U_ZPOSM_REG, (value>>6)&0x00FF);
	return getZero();
}




uint16_t AS5047U::nop16(){
	uint16_t result = spi_transfer16(0xFFFF); // using 0xFFFF as nop instead of 0x0000, then next call to fastAngle will return an angle
	return result&AS5047U_RESULT_MASK;
}



uint16_t AS5047U::spi_transfer16(uint16_t outdata) {
	spi->beginTransaction(settings);
	if (nCS>=0)
		digitalWrite(nCS, 0);
	uint16_t result = spi->transfer16(outdata);
	if (nCS>=0)
		digitalWrite(nCS, 1);
	spi->endTransaction();
	errorflag = ((result&AS5047U_ERROR)>0);
	warningflag = ((result&AS5047U_WARNING)>0);
	return result;
}

uint8_t AS5047U::calcCRC(uint16_t data){
	uint8_t crc = 0xC4;  // Initial value
    for (int i = 0; i < 16; i++) {
        if ((crc ^ data) & 0x8000) {
            crc = (crc << 1) ^ 0x1D;
        } else {
            crc <<= 1;
        }
        data <<= 1;
    }
    return crc ^ 0xFF;
}

uint16_t AS5047U::writeRegister24(uint16_t reg, uint16_t data) {
	uint8_t buff[3];
	buff[0] = (reg>>8)&0x3F;
	buff[1] = reg&0xFF;
	buff[2] = calcCRC(reg);
	spi->beginTransaction(settings);
	if (nCS>=0)
		digitalWrite(nCS, 0);
	spi->transfer(buff, 3);
	if (nCS>=0)
		digitalWrite(nCS, 1);
	spi->endTransaction();
	errorflag = ((buff[0]&0x40)>0);
	warningflag = ((buff[0]&0x80)>0);

	buff[0] = (data>>8)&0x3F;
	buff[1] = data&0xFF;
	buff[2] = calcCRC(data);
	spi->beginTransaction(settings);
	if (nCS>=0)
		digitalWrite(nCS, 0);
	spi->transfer(buff, 3);
	if (nCS>=0)
		digitalWrite(nCS, 1);
	spi->endTransaction();
	errorflag = ((buff[0]&0x40)>0);
	warningflag = ((buff[0]&0x80)>0);

	delayMicroseconds(50);

	return buff[0]<<8 | buff[1];
}