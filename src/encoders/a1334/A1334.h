

#ifndef __A1334_H__
#define __A1334_H__

#include <Arduino.h>
#include <SPI.h>


#define A1334_CPR 4096
#define A1334_BITORDER MSBFIRST

static SPISettings A1334SPISettings(8000000, A1334_BITORDER, SPI_MODE3); // @suppress("Invalid arguments")


typedef union {
	struct {
		uint16_t angle:12;
		uint16_t p:1;
		uint16_t nf:1;
		uint16_t ef:1;
		uint16_t ridc:1;
	};
	uint16_t reg;
} A1334Angle;


#define A1334_REG_ANG 0x2000




class A1334 {
public:
	A1334(SPISettings settings = A1334SPISettings, int nCS = -1);
	virtual ~A1334();

	virtual void init(SPIClass* _spi = &SPI);
	A1334Angle readRawAngle(); // 10 or 12 bit angle value    
protected:
	uint16_t spi_transfer16(uint16_t outdata);
	SPIClass* spi;
	SPISettings settings;
	int nCS = -1;
};





#endif