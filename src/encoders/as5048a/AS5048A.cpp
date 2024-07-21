/*
 * AS5048A.cpp
 *
 *  Created on: 8 Mar 2021
 *      Author: runger
 */

#include "AS5048A.h"

AS5048A::AS5048A(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {
	// nix
}

AS5048A::~AS5048A() {
}


void AS5048A::init(SPIClass* _spi) {
	spi = _spi;
	if (nCS>=0) {
		pinMode(nCS, OUTPUT);
		digitalWrite(nCS, HIGH);
	}
	//SPI has an internal SPI-device counter, it is possible to call "begin()" from different devices
	spi->begin();
	readRawAngle(); // read an angle
}

float AS5048A::getCurrentAngle(){
	readRawAngle();
	return ((float)readRawAngle())/(float)AS5048A_CPR * 2.0f * (float)PI;
}

float AS5048A::getFastAngle(){
	return ((float)readRawAngle())/(float)AS5048A_CPR * 2.0f * (float)PI;
}


uint16_t AS5048A::readRawAngle(){
	uint16_t command = AS5048A_ANGLE_REG | AS5048A_PARITY | AS5048A_RW; // set r=1 and parity=1, result ix 0xFFFF
	uint16_t lastresult = spi_transfer16(command)&AS5048A_RESULT_MASK;
	return lastresult;
}


uint16_t AS5048A::readMagnitude(){
	uint16_t command = AS5048A_MAGNITUDE_REG | AS5048A_RW; // set r=1, result ix 0x7FFE
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop();
	return result;
}


bool AS5048A::isErrorFlag(){
	return errorflag;
}


AS5048Error AS5048A::clearErrorFlag(){
	uint16_t command = AS5048A_ERROR_REG | AS5048A_RW; // set r=1, result ix 0x4001
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	uint16_t result = nop();
	AS5048Error err = {
			.parityError = ((result&0x0004)!=0x0000),
			.commandInvalid = ((result&0x0002)!=0x0000),
			.framingError = ((result&0x0001)!=0x0000)
	};
	return err;
}


AS5048Diagnostics AS5048A::readDiagnostics(){
	uint16_t command = AS5048A_DIAGNOSTICS_REG | AS5048A_RW; // set r=1, result ix 0x7FFD
	/*uint16_t cmdresult =*/ spi_transfer16(command);
	AS5048Diagnostics result = {
			.reg = nop()
	};
	return result;
}


uint16_t AS5048A::setZero(uint16_t){
	// TODO implement me!
	return 0;
}


uint16_t AS5048A::enableOneTimeProgramming(){
	// no plans to implement this at the moment. one-time-programming a $10 chip isn't really a "maker" thing to do. The zero
	// position can be easily retained in software, stored on the MCU, and thereby the sensor can be reused in another project.
	return 0;
}


uint16_t AS5048A::programZero(){
	// no plans to implement this at the moment. one-time-programming a $10 chip isn't really a "maker" thing to do. The zero
	// position can be easily retained in software, stored on the MCU, and thereby the sensor can be reused in another project.
	return 0;
}

uint16_t AS5048A::nop(){
	uint16_t result = spi_transfer16(0xFFFF); // using 0xFFFF as nop instead of 0x0000, then next call to fastAngle will return an angle
	return result&AS5048A_RESULT_MASK;
}

uint16_t AS5048A::spi_transfer16(uint16_t outdata) {
	spi->beginTransaction(settings);
	if (nCS>=0)
		digitalWrite(nCS, 0);
	uint16_t result = spi->transfer16(outdata);
	if (nCS>=0)
		digitalWrite(nCS, 1);
	spi->endTransaction();
	// TODO check parity
	errorflag = ((result&AS5048A_ERRFLG)>0);
	return result;
}


