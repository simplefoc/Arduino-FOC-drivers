
#ifndef __SC60228_H__
#define __SC60228_H__

#include "Arduino.h"
#include "SPI.h"


typedef union {
    struct {
        uint16_t parity:1;
        uint16_t df2:1;
        uint16_t df1:1;
        uint16_t err:1;
        uint16_t angle:12;
    };
    uint16_t reg;
} SC60228Angle;


#define SC60228_CPR 4096
#define SC60228_BITORDER MSBFIRST

static SPISettings SC60228SPISettings(8000000, SC60228_BITORDER, SPI_MODE1); // @suppress("Invalid arguments")



class SC60228 {
public:
    SC60228(SPISettings settings = SC60228SPISettings, int nCS = -1);
	virtual ~SC60228();

	virtual void init(SPIClass* _spi = &SPI);

    SC60228Angle readRawAngle(); // 12bit angle value + error flag
    bool isError(); // true if the last call to readRawAngle() returned an error

protected:
	uint16_t spi_transfer16(uint16_t outdata);

	SPIClass* spi;
	SPISettings settings;
	bool errorflag = false;
	int nCS = -1;
};




#endif