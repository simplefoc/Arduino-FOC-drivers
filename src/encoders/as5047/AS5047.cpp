/*
 * AS5047.cpp
 *
 *  Created on: 2 May 2021
 *      Author: runger
 */

#include "./AS5047.h"

AS5047::AS5047(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {
	// nix
}

AS5047::~AS5047() {
}


void AS5047::init(SPIClass* _spi) {
	spi = _spi;
	if (nCS>=0)
		pinMode(nCS, OUTPUT);
	digitalWrite(nCS, HIGH);
	//SPI has an internal SPI-device counter, it is possible to call "begin()" from different devices
	spi->begin();
	readRawAngle(); // read an angle
}

float AS5047::getCurrentAngle(){
	readCorrectedAngle();
	return ((float)readCorrectedAngle())/(float)AS5047_CPR * 2.0f * (float)PI;
}

float AS5047::getFastAngle(){
	return ((float)readCorrectedAngle())/(float)AS5047_CPR * 2.0f * (float)PI;
}


uint16_t AS5047::readRawAngle(){
	uint16_t command = AS5047_ANGLEUNC_REG | AS5047_RW; // set r=1 and parity=0, result is 0x7FFE
	uint16_t lastresult = spi_transfer16(command)&AS5047_RESULT_MASK;
	return lastresult;
}
uint16_t AS5047::readCorrectedAngle(){
	uint16_t command = AS5047_ANGLECOM_REG | AS5047_PARITY | AS5047_RW; // set r=1 and parity=1, result is 0xFFFF
	uint16_t lastresult = spi_transfer16(command)&AS5047_RESULT_MASK;
	return lastresult;
}


uint16_t AS5047::readMagnitude(){
	uint16_t command = AS5047_MAGNITUDE_REG | AS5047_RW; // set r=1, result is 0x7FFD
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop();
	return result;
}


bool AS5047::isErrorFlag(){
	return errorflag;
}


AS5047Error AS5047::clearErrorFlag(){
	uint16_t command = AS5047_ERROR_REG | AS5047_RW; // set r=1, result is 0x4001
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop();
	AS5047Error err = {
			.framingError = ((result&0x0001)!=0x0000),
			.commandInvalid = ((result&0x0002)!=0x0000),
			.parityError = ((result&0x0004)!=0x0000)
	};
	return err;
}


AS5047Settings1 AS5047::readSettings1(){
	uint16_t command = AS5047_SETTINGS1_REG | AS5047_PARITY | AS5047_RW; // set r=1, result is 0xC018
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047Settings1 result = {
			.reg = nop()
	};
	return result;
}


void AS5047::writeSettings1(AS5047Settings1 settings){
	uint16_t command = AS5047_SETTINGS1_REG;  // set r=0, result is 0x0018
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	/*cmdresult =*/ spi_transfer16(settings.reg);
}


AS5047Settings2 AS5047::readSettings2(){
	uint16_t command = AS5047_SETTINGS2_REG | AS5047_RW; // set r=1, result is 0x4019
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047Settings2 result = {
			.reg = nop()
	};
	return result;
}


AS5047Diagnostics AS5047::readDiagnostics(){
	uint16_t command = AS5047_DIAGNOSTICS_REG | AS5047_PARITY | AS5047_RW; // set r=1, result is 0xFFFC
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5047Diagnostics result = {
			.reg = nop()
	};
	return result;
}


void AS5047::enablePWM(bool enable){
	AS5047Settings1 settings = readSettings1();
	settings.pwmon = enable;
	writeSettings1(settings);
}

void AS5047::enableABI(bool enable){
	AS5047Settings1 settings = readSettings1();
	settings.uvw_abi = enable?0:1;
	writeSettings1(settings);
}



uint16_t AS5047::setZero(uint16_t value){
	// TODO implement me!
	return 0;
}


uint16_t AS5047::nop(){
	uint16_t result = spi_transfer16(0xFFFF); // using 0xFFFF as nop instead of 0x0000, then next call to fastAngle will return an angle
	return result&AS5047_RESULT_MASK;
}

uint16_t AS5047::spi_transfer16(uint16_t outdata) {
	if (nCS>=0)
		digitalWrite(nCS, 0);
	spi->beginTransaction(settings);
	uint16_t result = spi->transfer16(outdata);
	spi->endTransaction();
	if (nCS>=0)
		digitalWrite(nCS, 1);
	// TODO check parity
	errorflag = ((result&AS5047_ERRFLG)>0);
	return result;
}


