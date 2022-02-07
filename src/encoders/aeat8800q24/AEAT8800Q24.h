#ifndef __AEAT8800Q24_H__
#define __AEAT8800Q24_H__


#include "Arduino.h"
#include "SPI.h"

#define AEAT8800Q24_CPR 65536

#define AEAT8800Q24_REG_CUST0 0x00
#define AEAT8800Q24_REG_CUST1 0x01
#define AEAT8800Q24_REG_ZERO_LSB 0x02
#define AEAT8800Q24_REG_ZERO_MSB 0x03
#define AEAT8800Q24_REG_CONF0 0x04
#define AEAT8800Q24_REG_CONF1 0x05
#define AEAT8800Q24_REG_CONF2 0x06
#define AEAT8800Q24_REG_CONF_OTP 0x13
#define AEAT8800Q24_REG_CAL_OTP 0x1B
#define AEAT8800Q24_REG_CUST_OTP 0x11
#define AEAT8800Q24_REG_CAL 0x17

#define AEAT8800Q24_CONF_OTP_ON 0xA3
#define AEAT8800Q24_CAL_OTP_ON 0xA5
#define AEAT8800Q24_CUST_OTP_ON 0xA1
#define AEAT8800Q24_CAL_ON 0x02
#define AEAT8800Q24_CAL_OFF 0x00


typedef union {
	struct {
		uint8_t uvw_pwm_mode:1;
		uint8_t pwm:2;
		uint8_t iwidth:2;
		uint8_t uvw:3;
	};
	uint8_t reg;
} AEAT8800Q24_CONF0_t;


typedef union {
	struct {
		uint8_t cpr1:4;
		uint8_t hys:4;
	};
	uint8_t reg;
} AEAT8800Q24_CONF1_t;


typedef union {
	struct {
		uint8_t dir:1;
		uint8_t zero_latency:1;
		uint8_t abs_res:2;
		uint8_t cpr2:4;
	};
	uint8_t reg;
} AEAT8800Q24_CONF2_t;



typedef union {
	struct {
		uint8_t mhi:1;
		uint8_t mlo:1;
		uint8_t ready:1;
		uint8_t parity:1;
		uint8_t :4;
	};
	uint8_t reg;
} AEAT8800Q24_Status_t;


#ifndef MSBFIRST
#define MSBFIRST BitOrder::MSBFIRST
#endif

static SPISettings AEAT8800Q24SPISettings(1000000, MSBFIRST, SPI_MODE3); // @suppress("Invalid arguments")
static SPISettings AEAT8800Q24SSISettings(4000000, MSBFIRST, SPI_MODE2); // @suppress("Invalid arguments")


class AEAT8800Q24 {
public:
	AEAT8800Q24(int nCS, int pinNSL = MOSI, SPISettings spiSettings = AEAT8800Q24SPISettings, SPISettings ssiSettings = AEAT8800Q24SSISettings);
	virtual ~AEAT8800Q24();

	virtual void init(SPIClass* _spi = &SPI);

	float getCurrentAngle(); // angle in radians, return current value

	uint16_t readRawAngle(); // 14bit angle value
	AEAT8800Q24_Status_t getLastStatus(); // get status associated with last angle read

    uint16_t getZero();
    void setZero(uint16_t);

    AEAT8800Q24_CONF0_t getConf0();
    void setConf0(AEAT8800Q24_CONF0_t);

    AEAT8800Q24_CONF1_t getConf1();
    void setConf1(AEAT8800Q24_CONF1_t);

    AEAT8800Q24_CONF2_t getConf2();
    void setConf2(AEAT8800Q24_CONF2_t);

private:
	int nCS = -1;
    int pinNSL = -1;
	SPISettings spiSettings;
	SPISettings ssiSettings;
	SPIClass* spi;
	AEAT8800Q24_Status_t lastStatus;

    uint16_t transfer16SPI(uint16_t outValue);
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
};


#endif