/*
 * AS5048A.h
 *
 *  Created on: 8 Mar 2021
 *      Author: runger
 */

#ifndef AS5048A_H_
#define AS5048A_H_

#include "Arduino.h"
#include "SPI.h"




union AS5048Diagnostics {
	struct {
		uint16_t agc:8;
		uint16_t ocf:1;
		uint16_t cof:1;
		uint16_t compLow:1;
		uint16_t compHigh:1;
	};
	uint16_t reg;
};


struct AS5048Error {
	bool parityError;
	bool commandInvalid;
	bool framingError;
};


#define AS5048A_CPR 16384
#define AS5048A_ANGLE_REG 0x3FFF
#define AS5048A_ERROR_REG 0x0001
#define AS5048A_PROGCTL_REG 0x0003
#define AS5048A_OTPHIGH_REG 0x0016
#define AS5048A_OTPLOW_REG 0x0017
#define AS5048A_DIAGNOSTICS_REG 0x3FFD
#define AS5048A_MAGNITUDE_REG 0x3FFE
#define AS5048A_PARITY 0x8000
#define AS5048A_RW 0x4000
#define AS5048A_ERRFLG 0x4000
#define AS5048A_RESULT_MASK 0x3FFF


#define AS5048_BITORDER MSBFIRST


static SPISettings AS5048SPISettings(8000000, AS5048_BITORDER, SPI_MODE1); // @suppress("Invalid arguments")


class AS5048A {
public:
	AS5048A(SPISettings settings = AS5048SPISettings, int nCS = -1);
	virtual ~AS5048A();

	virtual void init(SPIClass* _spi = &SPI);

	float getCurrentAngle(); // angle in radians, return current value
	float getFastAngle();	 // angle in radians, return last value and read new

	uint16_t readRawAngle(); // 14bit angle value
	uint16_t readMagnitude(); // 14bit magnitude value

	bool isErrorFlag();
	AS5048Error clearErrorFlag();

	AS5048Diagnostics readDiagnostics();

	uint16_t setZero(uint16_t);
	uint16_t enableOneTimeProgramming();
	uint16_t programZero();

private:

	uint16_t nop();
	uint16_t spi_transfer16(uint16_t outdata);
	SPIClass* spi;
	SPISettings settings;
	bool errorflag = false;
	int nCS = -1;

};

#endif /* AS5048A_H_ */
