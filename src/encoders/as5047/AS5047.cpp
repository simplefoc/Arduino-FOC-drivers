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
	if (nCS>=0) {
		pinMode(nCS, OUTPUT);
		digitalWrite(nCS, HIGH);
	}
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
	spi_transfer16(command);
	spi_transfer16(calcParity(settings.reg));
}


AS5047Settings2 AS5047::readSettings2(){
	uint16_t command = AS5047_SETTINGS2_REG | AS5047_RW; // set r=1, result is 0x4019
	spi_transfer16(command);
	AS5047Settings2 result = {
			.reg = nop()
	};
	return result;
}


void AS5047::writeSettings2(AS5047Settings2 settings){
	uint16_t command = AS5047_SETTINGS2_REG | AS5047_PARITY; // set r=0, result is 0x8019
	spi_transfer16(command);
	spi_transfer16(calcParity(settings.reg));
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
	if (settings.pwmon == enable) return; // no change
	settings.pwmon = enable;
	writeSettings1(settings);
}

void AS5047::enableABI(bool enable){
	AS5047Settings1 settings = readSettings1();
	uint8_t val = enable?0:1;
	if (settings.uvw_abi == val) return; // no change
	settings.uvw_abi = val;
	writeSettings1(settings);
}


void AS5047::enableDEAC(bool enable){
	AS5047Settings1 settings = readSettings1();
	uint8_t val = enable?0:1;
	if (settings.daecdis == val) return; // no change
	settings.daecdis = enable?0:1;
	writeSettings1(settings);
}


void AS5047::useCorrectedAngle(bool useCorrected){
	AS5047Settings1 settings = readSettings1();
	uint8_t val = useCorrected?0:1;
	if (settings.dataselect == val) return; // no change
	settings.dataselect = val;
	writeSettings1(settings);
}



void AS5047::setHysteresis(uint8_t hyst){
	if (hyst>3) hyst = 3;
	uint8_t val = 3-hyst;
	AS5047Settings2 settings = readSettings2();
	if (settings.hys == val) return; // no change
	settings.hys = val;
	writeSettings2(settings);
}




void AS5047::setABIResolution(AS5047ABIRes res){
	AS5047Settings1 settings = readSettings1();
	uint8_t val = (res>>3)&0x01;
	if (settings.abibin!=val || settings.uvw_abi!=0) {
		settings.abibin = val;
		settings.uvw_abi = 0;
		writeSettings1(settings);
	}
	AS5047Settings2 settings2 = readSettings2();
	val = (res&0x07);
	if (settings2.abires!=val) {
		settings2.abires = val;
		writeSettings2(settings2);
	}
} 



uint16_t AS5047::setZero(uint16_t value){
	uint16_t command = AS5047_ZPOSL_REG | AS5047_PARITY | AS5047_RW;
	spi_transfer16(command);
	AS5047ZPosL posL = {
			.reg = nop()
	};
	command = AS5047_ZPOSM_REG | AS5047_PARITY;
	spi_transfer16(command);
	spi_transfer16(calcParity((value>>6)&0x00FF));
	command = AS5047_ZPOSL_REG;
	posL.zposl = value&0x003F;
	spi_transfer16(command);
	spi_transfer16(calcParity(posL.reg));

	return getZero();
}


uint16_t AS5047::getZero() {
	uint16_t command = AS5047_ZPOSM_REG | AS5047_RW;
	spi_transfer16(command);
	command = AS5047_ZPOSL_REG | AS5047_PARITY | AS5047_RW;
	uint16_t result = spi_transfer16(command);
	AS5047ZPosL posL = {
			.reg = nop()
	};
	return ((result&0x00FF)<<6) | posL.zposl;
}


uint16_t AS5047::nop(){
	uint16_t result = spi_transfer16(0xFFFF); // using 0xFFFF as nop instead of 0x0000, then next call to fastAngle will return an angle
	return result&AS5047_RESULT_MASK;
}


uint16_t AS5047::calcParity(uint16_t data){
	data = data & 0x7FFF;
	int sum = 0;
	for (int i=0;i<15;i++)
		if ((data>>i)&0x0001)
			sum++;
	return (sum&0x01)==0x01?(data|0x8000):data;
}


uint16_t AS5047::spi_transfer16(uint16_t outdata) {
	spi->beginTransaction(settings);
	if (nCS>=0)
		digitalWrite(nCS, 0);
	uint16_t result = spi->transfer16(outdata);
	if (nCS>=0)
		digitalWrite(nCS, 1);
	spi->endTransaction();
	// TODO check parity
	errorflag = ((result&AS5047_ERRFLG)>0);
	return result;
}


