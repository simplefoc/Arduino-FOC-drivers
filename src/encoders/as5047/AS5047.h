/*
 * AS5047.h
 *
 *  Created on: 2 May 2021
 *      Author: runger
 */

#ifndef AS5047_H_
#define AS5047_H_

#include "Arduino.h"
#include "SPI.h"




union AS5047Diagnostics {
	struct {
		uint16_t agc:8;
		uint16_t lf:1;
		uint16_t cof:1;
		uint16_t magh:1;
		uint16_t magl:1;
	};
	uint16_t reg;
};


union AS5047ZPosM {
	struct {
		uint16_t zposm:8;
	};
	uint16_t reg;
};


union AS5047ZPosL {
	struct {
		uint16_t zposl:6;
		uint16_t comp_l_error_en:1;
		uint16_t comp_h_error_en:1;
	};
	uint16_t reg;
};


union AS5047Settings1 {
	struct {
		uint16_t reserved:1;
		uint16_t noiseset:1;
		uint16_t dir:1;
		uint16_t uvw_abi:1;
		uint16_t daecdis:1;
		uint16_t abibin:1;
		uint16_t dataselect:1;
		uint16_t pwmon:1;
	};
	uint16_t reg;
};


union AS5047Settings2 {
	struct {
		uint16_t uvwpp:3;
		uint16_t hys:2;
		uint16_t abires:3;
	};
	uint16_t reg;
};


struct AS5047Error {
	bool framingError;
	bool commandInvalid;
	bool parityError;
};


#define AS5047_CPR 16384
#define AS5047_ANGLECOM_REG 0x3FFF
#define AS5047_ANGLEUNC_REG 0x3FFE
#define AS5047_MAGNITUDE_REG 0x3FFD
#define AS5047_DIAGNOSTICS_REG 0x3FFC
#define AS5047_ERROR_REG 0x0001
#define AS5047_PROG_REG 0x0003
#define AS5047_ZPOSM_REG 0x0016
#define AS5047_ZPOSL_REG 0x0017
#define AS5047_SETTINGS1_REG 0x0018
#define AS5047_SETTINGS2_REG 0x0019

#define AS5047_PARITY 0x8000
#define AS5047_RW 0x4000
#define AS5047_ERRFLG 0x4000
#define AS5047_RESULT_MASK 0x3FFF


#define AS5047_BITORDER MSBFIRST

static SPISettings AS5047SPISettings(8000000, AS5047_BITORDER, SPI_MODE1); // @suppress("Invalid arguments")


class AS5047 {
public:
	AS5047(SPISettings settings = AS5047SPISettings, int nCS = -1);
	virtual ~AS5047();

	virtual void init(SPIClass* _spi = &SPI);

	float getCurrentAngle(); // angle in radians, return current value
	float getFastAngle();	 // angle in radians, return last value and read new

	uint16_t readRawAngle(); // 14bit angle value
	uint16_t readCorrectedAngle(); // 14bit corrected angle value
	uint16_t readMagnitude(); // 14bit magnitude value

	bool isErrorFlag();
	AS5047Error clearErrorFlag();

	AS5047Diagnostics readDiagnostics();
	AS5047Settings1 readSettings1();
	void writeSettings1(AS5047Settings1 settings);
	AS5047Settings2 readSettings2();
	void enablePWM(bool enable);
	void enableABI(bool enable);

	uint16_t setZero(uint16_t);

private:

	uint16_t nop();
	uint16_t spi_transfer16(uint16_t outdata);
	SPIClass* spi;
	SPISettings settings;
	bool errorflag = false;
	int nCS = -1;

};

#endif /* AS5047_H_ */
