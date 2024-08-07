#ifndef __MA735_H__
#define __MA735_H__


#include "Arduino.h"
#include "SPI.h"

enum FieldStrength : uint8_t {
    FS_NORMAL = 0x00,
    FS_LOW = 0x01,
    FS_HIGH = 0x02,
    FS_ERR = 0x03 // impossible state
};


#define _2PI 6.28318530718f
#define MA735_16BIT 65536.0f

#define MA735_REG_ZERO_POSITION_LSB 0x00
#define MA735_REG_ZERO_POSITION_MSB 0x01
#define MA735_REG_BCT 0x02
#define MA735_REG_ET 0x03
#define MA735_REG_ILIP_PPT_LSB 0x04
#define MA735_REG_PPT_MSB 0x05
#define MA735_REG_MGLT_MGHT 0x06
#define MA735_REG_RD 0x09
#define MA735_REG_FW 0x0E
#define MA735_REG_HYS 0x10
#define MA735_REG_MGH_MGL 0x1B

#define MA735_BITORDER MSBFIRST

static SPISettings MA735SPISettings(1000000, MA735_BITORDER, SPI_MODE3); // @suppress("Invalid arguments")
static SPISettings MA735SSISettings(4000000, MA735_BITORDER, SPI_MODE1); // @suppress("Invalid arguments")


class MA735 {
public:
	MA735(SPISettings settings = MA735SPISettings, int nCS = -1);
	virtual ~MA735();

	virtual void init(SPIClass* _spi = &SPI);

	float getCurrentAngle(); // angle in radians, return current value

	uint16_t readRawAngle(); // 9-13bit angle value
    uint16_t readRawAngleSSI(); // 9-13bit angle value

    uint16_t getZero();
    uint8_t getBiasCurrentTrimming();
    bool isBiasCurrrentTrimmingX();
    bool isBiasCurrrentTrimmingY();
    uint16_t getPulsesPerTurn();
    uint8_t getIndexLength();
    uint8_t getRotationDirection();
    uint8_t getFieldStrengthHighThreshold();
    uint8_t getFieldStrengthLowThreshold();
    FieldStrength getFieldStrength();
    uint8_t getFilterWindow();
    uint8_t getHysteresis();
    float getResolution();
    int getUpdateTime();

    void setZero(uint16_t);
    void setBiasCurrentTrimming(uint8_t);
    void setBiasCurrrentTrimmingEnabled(bool Xenabled, bool Yenabled);
    void setPulsesPerTurn(uint16_t);
    void setIndexLength(uint8_t);
    void setRotationDirection(uint8_t);
    void setFieldStrengthThresholds(uint8_t high, uint8_t low);
    void setFilterWindow(uint8_t);
    void setHysteresis(uint8_t);
    void setResolution(float resolution);
    void setUpdateTime(int microsec);

private:
	SPIClass* spi;
	SPISettings settings;
	int nCS = -1;
    //float MA735_CPR = 65536.0f;

    uint16_t transfer16(uint16_t outValue);
    uint8_t readRegister(uint8_t reg);
    uint8_t writeRegister(uint8_t reg, uint8_t value);
};

#endif
