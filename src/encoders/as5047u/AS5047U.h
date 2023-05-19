/*
 * AS5047.h
 *
 *  Created on: 24 Feb 2022
 *      Author: runger
 */

#ifndef AS5047U_H_
#define AS5047U_H_

#include "Arduino.h"
#include "SPI.h"


union AS5047UDisableSettings {
	struct {
		uint16_t :9;
		uint16_t filter_disable:1;
		uint16_t :4;
		uint16_t abi_off:1;
		uint16_t uvw_off:1;
	};
	uint16_t reg;
};



union AS5047UDiagnostics {
	struct {
		uint16_t spi_cnt:2;
		uint16_t :1;
		uint16_t agc_finished:1;
		uint16_t off_finished:1;
		uint16_t sin_finished:1;
		uint16_t cos_finished:1;
		uint16_t maghalf_flag:1;
		uint16_t comp_h:1;
		uint16_t comp_l:1;
		uint16_t cordic_ovf:1;
		uint16_t loops_finished:1;
		uint16_t vdd_mode:1;
	};
	uint16_t reg;
};


union AS5047UZPosM {
	struct {
		uint16_t zposm:8;
	};
	uint16_t reg;
};


union AS5047UZPosL {
	struct {
		uint16_t zposl:6;
		uint16_t dia1_en:1; // automotive version only
		uint16_t dia2_en:1; // automotive version only
	};
	uint16_t reg;
};


union AS5047USettings1 {
	struct {
		uint16_t k_max:3;
		uint16_t k_min:3;
		uint16_t dia3_en:1; // not applicable
		uint16_t dia4_en:1; // not applicable
	};
	uint16_t reg;
};


union AS5047USettings2 {
	struct {
		uint16_t iwidth:1;
		uint16_t noiseset:1;
		uint16_t dir:1;
		uint16_t uvw_abi:1;
		uint16_t daecdis:1;
		uint16_t abi_dec:1;
		uint16_t data_select:1;
		uint16_t pwm_on:1;		
	};
	uint16_t reg;
};




union AS5047USettings3 {
	struct {
		uint16_t uvwpp:3;
		uint16_t hys:2;
		uint16_t abires:3;
	};
	uint16_t reg;
};




union AS5047UECCSettings {
	struct {
		uint16_t ecc_chsum:7;
		uint16_t ecc_en:1;
	};
	uint16_t reg;
};




struct AS5047UError {
	struct {
		uint16_t cordic_ovf:1;
		uint16_t off_notfinished:1;
		uint16_t :1;
		uint16_t wdtst:1;
		uint16_t crc_error:1;
		uint16_t cmd_error:1;
		uint16_t frame_error:1;
		uint16_t p2ram_error:1;
		uint16_t p2ram_warning:1;
		uint16_t maghalf:1;
		uint16_t agc_warning:1;		
	};
	uint16_t reg;
};





#define AS5047U_CPR 16384
#define AS5047U_ANGLECOM_REG 0x3FFF
#define AS5047U_ANGLEUNC_REG 0x3FFE
#define AS5047U_MAGNITUDE_REG 0x3FFD
#define AS5047U_VELOCITY_REG 0x3FFC
#define AS5047U_SIN_REG 0x3FFA
#define AS5047U_COS_REG 0x3FFB
#define AS5047U_AGC_REG 0x3FF9
#define AS5047U_DIAGNOSTICS_REG 0x3FF5
#define AS5047U_ERROR_REG 0x0001
#define AS5047U_PROG_REG 0x0003
#define AS5047U_ECCCHK_REG 0x00D1

#define AS5047U_DISABLE_REG 0x0015
#define AS5047U_ZPOSM_REG 0x0016
#define AS5047U_ZPOSL_REG 0x0017
#define AS5047U_SETTINGS1_REG 0x0018
#define AS5047U_SETTINGS2_REG 0x0019
#define AS5047U_SETTINGS3_REG 0x001A
#define AS5047U_ECC_REG 0x001B

#define AS5047U_WARNING 0x8000
#define AS5047U_ERROR 0x4000
#define AS5047U_RW 0x4000
#define AS5047U_ERRFLG 0xC000
#define AS5047U_RESULT_MASK 0x3FFF


#define AS5047U_BITORDER MSBFIRST

static SPISettings AS5047USPISettings(8000000, AS5047U_BITORDER, SPI_MODE1); // @suppress("Invalid arguments")


class AS5047U {
public:
	AS5047U(SPISettings settings = AS5047USPISettings, int nCS = -1);
	virtual ~AS5047U();

	virtual void init(SPIClass* _spi = &SPI);

	float getCurrentAngle(); // angle in radians, return current value
	float getFastAngle();	 // angle in radians, return last value and read new

	uint16_t readRawAngle(); // 14bit angle value
	uint16_t readCorrectedAngle(); // 14bit corrected angle value
	uint16_t readMagnitude(); // 14bit magnitude value
	uint16_t readVelocity(); // 14bit magnitude value

	bool isErrorFlag();
	bool isWarningFlag();
	AS5047UError clearErrorFlag();

	AS5047UDiagnostics readDiagnostics();
	uint8_t readAGC();
	uint8_t readECCCHK();

	AS5047UDisableSettings readDisableSettings();
	void writeDisableSettings(AS5047UDisableSettings settings);
	AS5047USettings1 readSettings1();
	void writeSettings1(AS5047USettings1 settings);
	AS5047USettings2 readSettings2();
	void writeSettings2(AS5047USettings2 settings);
	AS5047USettings3 readSettings3();
	void writeSettings3(AS5047USettings3 settings);
	AS5047UECCSettings readECCSettings();
	void writeECCSettings(AS5047UECCSettings settings);

	void enablePWM(bool enable, bool pwmOnWPin = true);
	void enableABI(bool enable);
	void enableUVW(bool enable);

	uint16_t setZero(uint16_t);
	uint16_t getZero();

private:

	uint16_t nop16();
	uint16_t spi_transfer16(uint16_t outdata);
	uint8_t calcCRC(uint16_t data);
	uint16_t writeRegister24(uint16_t reg, uint16_t data);
	SPIClass* spi;
	SPISettings settings;
	bool errorflag = false;
	bool warningflag = false;
	int nCS = -1;

};

#endif /* AS5047U_H_ */
