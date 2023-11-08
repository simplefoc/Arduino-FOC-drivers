

#pragma once



#include "Arduino.h"
#include "SPI.h"

#define MT6835_OP_READ  0b0011
#define MT6835_OP_WRITE 0b0110
#define MT6835_OP_PROG  0b1100
#define MT6835_OP_ZERO  0b0101
#define MT6835_OP_ANGLE 0b1010

#define MT6835_CMD_MASK  0b111100000000000000000000
#define MT6835_ADDR_MASK 0b000011111111111100000000
#define MT6835_DATA_MASK 0b000000000000000011111111

#define MT6835_CPR 2097152

#define MT6835_STATUS_OVERSPEED 0x01
#define MT6835_STATUS_WEAKFIELD 0x02
#define MT6835_STATUS_UNDERVOLT 0x04
#define MT6835_CRC_ERROR 0x08

#define MT6835_WRITE_ACK 0x55

#define MT6835_REG_USERID 0x001

#define MT6835_REG_ANGLE1 0x003
#define MT6835_REG_ANGLE2 0x004
#define MT6835_REG_ANGLE3 0x005
#define MT6835_REG_ANGLE4 0x006

#define MT6835_REG_ABZ_RES1 0x007
#define MT6835_REG_ABZ_RES2 0x008

#define MT6835_REG_ZERO1 0x009
#define MT6835_REG_ZERO2 0x00A

#define MT6835_REG_OPTS0 0x00A
#define MT6835_REG_OPTS1 0x00B
#define MT6835_REG_OPTS2 0x00C
#define MT6835_REG_OPTS3 0x00D
#define MT6835_REG_OPTS4 0x00E
#define MT6835_REG_OPTS5 0x011

// NLC table, 192 bytes
#define MT6835_REG_NLC_BASE 0x013

#define MT6835_REG_CAL_STATUS 0x113


union MT6835ABZRes {
	struct {
		uint8_t ab_swap:1;
		uint8_t abz_off:1;
		uint8_t abz_res_low:6;
	};
	uint8_t reg;
};



union MT6835Options0 {
	struct {
		uint8_t z_pul_wid:3;
		uint8_t z_edge:1;
		uint8_t zero_pos_low:4;
	};
	uint8_t reg;
};



union MT6835Options1 {
	struct {
		uint8_t uvw_res:4;
		uint8_t uvw_off:1;
		uint8_t uvw_mux:1;
		uint8_t z_phase:2;
	};
	uint8_t reg;
};



union MT6835Options2 {
	struct {
		uint8_t pwm_sel:3;
		uint8_t pwm_pol:1;
		uint8_t pwm_fq:1;
		uint8_t nlc_en:1;
		uint8_t reserved:2;
	};
	uint8_t reg;
};



union MT6835Options3 {
	struct {
		uint8_t hyst:3;
		uint8_t rot_dir:1;
		uint8_t reserved:4;
	};
	uint8_t reg;
};



union MT6835Options4 {
	struct {
		uint8_t reserved:4;
		uint8_t autocal_freq:3;
		uint8_t gpio_ds:1;
	};
	uint8_t reg;
};



union MT6835Options5 {
	struct {
		uint8_t bw:3;
		uint8_t reserved:5;
	};
	uint8_t reg;
};




union MT6835Command {
	struct {
		uint32_t unused:8;
		uint32_t data:8;
		uint32_t addr:12;
		uint32_t cmd:4;
	};
	uint32_t val;
};




#define MT6835_BITORDER MSBFIRST

static SPISettings MT6835SPISettings(1000000, MT6835_BITORDER, SPI_MODE3); // @suppress("Invalid arguments")




class MT6835 {
public:
    MT6835(SPISettings settings = MT6835SPISettings, int nCS = -1);
    virtual ~MT6835();

    virtual void init(SPIClass* _spi = &SPI);


	float getCurrentAngle(); // angle in radians, return current value

	uint32_t readRawAngle21(); // up to 21bit precision angle value

    
    uint8_t getBandwidth();
    void setBandwidth(uint8_t bw);

    uint8_t getHysteresis();
    void setHysteresis(uint8_t hyst);

    uint8_t getRotationDirection();
    void setRotationDirection(uint8_t dir);

    uint16_t getABZResolution();
    void setABZResolution(uint16_t res);

    bool isABZEnabled();
    void setABZEnabled(bool enabled);

    bool isABSwapped();
    void setABSwapped(bool swapped);

    uint16_t getZeroPosition();
    void setZeroPosition(uint16_t pos);

    MT6835Options1 getOptions1();
    void setOptions1(MT6835Options1 opts);

    MT6835Options2 getOptions2();
    void setOptions2(MT6835Options2 opts);

    MT6835Options3 getOptions3();
    void setOptions3(MT6835Options3 opts);

    MT6835Options4 getOptions4();
    void setOptions4(MT6835Options4 opts);

	uint8_t getStatus();

	uint8_t getCalibrationStatus();

	bool setZeroFromCurrentPosition();
	bool writeEEPROM(); // wait 6s after calling this method

	bool checkcrc = false;
	
private:
	SPIClass* spi;
	SPISettings settings;
	int nCS = -1;
	uint8_t laststatus = 0;
	uint8_t lastcrc = 0;

    void transfer24(MT6835Command* outValue);
    uint8_t readRegister(uint16_t reg);
    bool writeRegister(uint16_t reg, uint8_t value);
	uint8_t calcCrc(uint32_t angle, uint8_t status);

};