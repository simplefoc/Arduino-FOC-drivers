#ifndef __MA330_H__
#define __MA330_H__


#include "Arduino.h"
#include "SPI.h"

enum FieldStrength : uint8_t {
    FS_NORMAL = 0x00,
    FS_LOW = 0x01,
    FS_HIGH = 0x02,
    FS_ERR = 0x03 // impossible state
};


#define _2PI 6.28318530718f
#define MA330_CPR 65536.0f

#define MA330_REG_ZERO_POSITION_LSB 0x00
#define MA330_REG_ZERO_POSITION_MSB 0x01
#define MA330_REG_BCT 0x02
#define MA330_REG_ET 0x03
#define MA330_REG_ILIP_PPT_LSB 0x04
#define MA330_REG_PPT_MSB 0x05
#define MA330_REG_MGLT_MGHT 0x06
#define MA330_REG_NPP 0x07
#define MA330_REG_RD 0x09
#define MA330_REG_FW 0x0E
#define MA330_REG_HYS 0x10
#define MA330_REG_MGH_MGL 0x1B

#define MA330_BITORDER MSBFIRST

static SPISettings MA330SPISettings(1000000, MA330_BITORDER, SPI_MODE3); // @suppress("Invalid arguments")

class MA330 {
public:
	MA330(SPISettings settings = MA330SPISettings, int nCS = -1);
	virtual ~MA330();

	virtual void init(SPIClass* _spi = &SPI);

	float getCurrentAngle(); // angle in radians, return current value

	uint16_t readRawAngle(); // 9-14bit angle value

    uint16_t getZero();
    uint8_t getBiasCurrentTrimming();
    bool isBiasCurrrentTrimmingX();
    bool isBiasCurrrentTrimmingY();
    uint16_t getPulsesPerTurn();
    uint8_t getIndexLength();
    uint8_t getNumberPolePairs();
    uint8_t getRotationDirection();
    uint8_t getFilterWidth();
    uint8_t getHysteresis();
    uint8_t getFieldStrengthHighThreshold();
    uint8_t getFieldStrengthLowThreshold();
    FieldStrength getFieldStrength();

    void setZero(uint16_t);
    void setBiasCurrentTrimming(uint8_t);
    void setBiasCurrrentTrimmingEnabled(bool Xenabled, bool Yenabled);
    void setPulsesPerTurn(uint16_t);
    void setIndexLength(uint8_t);
    void setNumberPolePairs(uint8_t);
    void setRotationDirection(uint8_t);
    void setFilterWidth(uint8_t);
    void setHysteresis(uint8_t);
    void setFieldStrengthThresholds(uint8_t high, uint8_t low);

private:
	SPIClass* spi;
	SPISettings settings;
	int nCS = -1;

    uint16_t transfer16(uint16_t outValue);
    uint8_t readRegister(uint8_t reg);
    uint8_t writeRegister(uint8_t reg, uint8_t value);
};

#endif