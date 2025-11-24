

#pragma once

#include "Arduino.h"
#include "SPI.h"

#define KTH7812_ANGLE_MASK  0b1110000000000000
#define KTH7812_CMD_MASK    0b1100000000000000
#define KTH7812_ADDR_MASK   0b0011111100000000
#define KTH7812_DATA_MASK   0b0000000011111111

#define KTH7812_CPR 65536
#define KTH7812_BITORDER MSBFIRST

#define KTH7812_REG_ZLSB 0x00
#define KTH7812_REG_ZMSB 0x01
#define KTH7812_REG_GAIN 0x02
#define KTH7812_REG_TRIM 0x03
#define KTH7812_REG_PPT0 0x04
#define KTH7812_REG_PPT1 0x05
#define KTH7812_REG_MGHL 0x06
#define KTH7812_REG_NPP  0x07
#define KTH7812_REG_ABZL 0x08
#define KTH7812_REG_RD   0x09

#define KTH7812_REG_WDIS 0x40

#define KTH7812_WRDIS_BIT 0b00000010
#define KTH7812_RD_BIT    0b10000000




union KTH7812Trim {
	struct {
		uint8_t xtrim:1;
		uint8_t ytrim:1;
		uint8_t reserved:6;
	};
	uint8_t reg;
};


union KTH7812_MGHL {
	struct {
        uint8_t reserved:2;
        uint8_t mgl:3;
        uint8_t mgh:3;
	};
	uint8_t reg;
};


union KTH7812_NPP {
    struct {
        uint8_t reserved:5;
        uint8_t npp:3;
    };
    uint16_t reg;
};


union KTH7812_PPT0 {
    struct {
        uint8_t reserved:2;
        uint8_t zd:2;
        uint8_t zl:2;
        uint8_t pptl:2;
    };
    uint16_t reg;
};


union KTH7812_ABZL {
    struct {
        uint8_t abzlimit:3;
        uint8_t reserved:5;
    };
    uint16_t reg;
};


#define KTH7812SPISettings SPISettings(1000000, KTH7812_BITORDER, SPI_MODE3)

#define _2PI 6.28318530718f


class KTH7812 {
public:
    KTH7812(SPISettings settings = KTH7812SPISettings, int nCS = -1, bool fastmode = false, bool withcrc = false);
    virtual ~KTH7812();

    virtual void init(SPIClass* _spi = &SPI);

	float getCurrentAngle(); // angle in radians, return current value

	int32_t readRawAngle16(); // 16 bit angle, no CRC
    int32_t readRawAngle12WithCRC(); // 12 bit angle with CRC

    uint8_t getGainTrim();
    void setGainTrim(uint8_t gain);

    KTH7812Trim getTrim();
    void setTrim(KTH7812Trim trim);

    KTH7812_MGHL getMGHL();
    void setMGHL(KTH7812_MGHL mgh);

    uint16_t getZero();
    void setZero(uint16_t zero);

    bool getDirection();
    void setDirection(bool rd);

    KTH7812_NPP getNPP();
    void setNPP(KTH7812_NPP npp);

    uint16_t getPPT();
    void setPPT(uint16_t ppt);

    uint8_t getABZLimit();
    void setABZLimit(uint8_t limit);

    KTH7812_PPT0 getZlZd();
    void setZlZd(KTH7812_PPT0 zlzd);


    bool fastmode = false;

private:
	SPIClass* spi;
	SPISettings settings;
	int nCS = -1;
    bool checkcrc = false;

	uint8_t laststatus = 0;
	uint8_t lastcrc = 0;

    uint16_t transfer16(uint16_t);
    uint8_t readRegister(uint8_t reg);
    uint8_t writeRegister(uint8_t reg, uint8_t value);
	uint8_t calcCrc(uint32_t angle, uint8_t status);

};

