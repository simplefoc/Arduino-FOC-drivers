
#pragma once


#include <Arduino.h>
#include <Wire.h>


#define AS5600_REG_ZMCO 0x00
#define AS5600_REG_ZPOS 0x01
#define AS5600_REG_MPOS 0x03
#define AS5600_REG_MANG 0x05
#define AS5600_REG_CONF 0x07

#define AS5600_REG_I2CADDR 0x20
#define AS5600_REG_I2CUPDT 0x21

#define AS5600_REG_ANGLE 0x0E
#define AS5600_REG_ANGLE_RAW 0x0C

#define AS5600_REG_STATUS 0x0B
#define AS5600_REG_AGC 0x1A
#define AS5600_REG_MAGNITUDE 0x1B

#define AS5600_REG_BURN 0xFF

#define AS5600_CPR (4096.0f)


union AS5600Conf {
	struct {
		uint16_t pm:2;
		uint16_t hyst:2;
		uint16_t outs:2;
		uint16_t pwmf:2;
		uint16_t sf:2;
		uint16_t fth:3;
		uint16_t wd:1;
		uint16_t unused:2;
	};
	uint16_t reg;
};

union AS5600Status {
	struct {
		uint8_t unused:3;
		uint8_t mh:1;
		uint8_t ml:1;
		uint8_t md:1;
		uint8_t unused2:2;
	};
	uint8_t reg;
};




class AS5600 {
public:
    AS5600(uint8_t address = 0x36);
    ~AS5600();

    virtual void init(TwoWire* wire = &Wire);

    // read an angle, either with or without hysteresis, depending on the useHysteresis flag
    // and using fast mode (not closing transactions) if so configured
    uint16_t angle();

    // read registers
    uint16_t readRawAngle();
    uint16_t readAngle();

    uint16_t readMagnitude();
    AS5600Status readStatus();
    uint8_t readAGC();

    AS5600Conf readConf();
    uint16_t readMang();
    uint16_t readMPos();
    uint16_t readZPos();
    uint8_t readZMCO();
    uint8_t readI2CAddr();

    // set registers
    void setConf(AS5600Conf value);
    void setMang(uint16_t value);
    void setMPos(uint16_t value);
    void setZPos(uint16_t value);
    void setI2CAddr(uint8_t value);
    void setI2CUpdt(uint8_t value);

    void burnSettings();

    bool closeTransactions = true;
    bool useHysteresis = true;
    uint8_t _address;
protected:
    TwoWire* _wire;

    void setAngleRegister();
    uint16_t readRegister(uint8_t reg, uint8_t len);
    void writeRegister(uint8_t reg, uint16_t val, uint8_t len = 2);
};

