

#pragma once



#include "Arduino.h"
#include "SPI.h"

#define MT6826S_OP_READ  0b0011
#define MT6826S_OP_WRITE 0b0110
#define MT6826S_OP_PROG  0b1100
#define MT6826S_OP_ANGLE 0b1010

#define MT6826S_CMD_MASK  0b111100000000000000000000
#define MT6826S_ADDR_MASK 0b000011111111111100000000
#define MT6826S_DATA_MASK 0b000000000000000011111111

#define MT6826S_CPR 32768

#define MT6826S_STATUS_OVERSPEED 0x01
#define MT6826S_STATUS_WEAKFIELD 0x02
#define MT6826S_STATUS_UNDERVOLT 0x04
#define MT6826S_CRC_ERROR 0x08

#define MT6826S_WRITE_ACK 0x55



#define MT6826S_REG_USERID 0x001

#define MT6826S_REG_ANGLE1 0x003
#define MT6826S_REG_ANGLE2 0x004
#define MT6826S_REG_ANGLE3 0x005
#define MT6826S_REG_ANGLE4 0x006

#define MT6826S_REG_ABZ_RES1 0x007
#define MT6826S_REG_ABZ_RES2 0x008

#define MT6826S_REG_ZERO1 0x009
#define MT6826S_REG_ZERO2 0x00A

#define MT6826S_REG_OPTS0 0x00A
#define MT6826S_REG_OPTS1 0x00B
#define MT6826S_REG_OPTS2 0x00C
#define MT6826S_REG_OPTS3 0x00D
#define MT6826S_REG_OPTS4 0x00E
#define MT6826S_REG_OPTS5 0x011

#define MT6826S_REG_POSINIT 	0x0D8
#define MT6826S_REG_CAL_STATUS 	0x113
#define MT6826S_REG_CALCTRL 	0x155

#define MT6826S_CALCTRL_KEY_EN 	0x5E


union MT6826SABZRes {
	struct {
		uint8_t ab_swap:1;
		uint8_t abz_off:1;
		uint8_t reserved:2;
		uint8_t abz_res_low:4;
	};
	uint8_t reg;
};



union MT6826SOptions0 {
	struct {
		uint8_t z_pul_wid:4;
		uint8_t zero_pos_low:4;
	};
	uint8_t reg;
};



union MT6826SOptions1 {
	struct {
		uint8_t uvw_res:4;
		uint8_t uvw_off:1;
		uint8_t uvw_mux:1;
		uint8_t z_phase:2;
	};
	uint8_t reg;
};



union MT6826SOptions2 {
	struct {
		uint8_t pwm_sel:3;
		uint8_t pwm_pol:1;
		uint8_t pwm_fq:1;
		uint8_t reserved:3;
	};
	uint8_t reg;
};



union MT6826SOptions3 {
	struct {
		uint8_t hyst:3;
		uint8_t rot_dir:1;
		uint8_t reserved:4;
	};
	uint8_t reg;
};



union MT6826SOptions4 {
	struct {
		uint8_t reserved:4;
		uint8_t autocal_freq:3;
		uint8_t gpio_ds:1;
	};
	uint8_t reg;
};



union MT6826SOptions5 {
	struct {
		uint8_t bw:3;
		uint8_t reserved:4;
		uint8_t z_edge:1;
	};
	uint8_t reg;
};



union MT6826SPosInit {
	struct {
		uint8_t reserved:1;
		uint8_t pos_init:1;
		uint8_t reserved2:6;
	};
	uint8_t reg;
};




union MT6826SCommand {
	struct {
		uint32_t unused:8;
		uint32_t data:8;
		uint32_t addr:12;
		uint32_t cmd:4;
	};
	uint32_t val;
};




#define MT6826S_BITORDER MSBFIRST

#define MT6826SSPISettings SPISettings(1000000, MT6826S_BITORDER, SPI_MODE3)




class MT6826S {
public:
    MT6826S(SPISettings settings = MT6826SSPISettings, int nCS = -1);
    virtual ~MT6826S();

    virtual void init(SPIClass* _spi = &SPI);


	float getCurrentAngle(); // angle in radians, return current value

	uint32_t readRawAngle15(); // up to 21bit precision angle value

    
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

    MT6826SOptions1 getOptions1();
    void setOptions1(MT6826SOptions1 opts);

    MT6826SOptions2 getOptions2();
    void setOptions2(MT6826SOptions2 opts);

    MT6826SOptions3 getOptions3();
    void setOptions3(MT6826SOptions3 opts);

    MT6826SOptions4 getOptions4();
    void setOptions4(MT6826SOptions4 opts);
	
    MT6826SOptions5 getOptions5();
    void setOptions5(MT6826SOptions5 opts);

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

    void transfer24(MT6826SCommand* outValue);
    uint8_t readRegister(uint16_t reg);
    bool writeRegister(uint16_t reg, uint8_t value);
	uint8_t calcCrc(uint32_t angle, uint8_t status);

};