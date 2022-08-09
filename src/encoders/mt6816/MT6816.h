
#ifndef MT6816_H
#define MT6816_H

#include "Arduino.h"
#include "SPI.h"

#define _2PI                                6.28318530718f
#define MT6816_CPR                          16384.0f

#define MT6816_READ_REG_03                  0x8300
#define MT6816_READ_REG_04                  0x8400

#define MT6816_NO_MAGNET_WARNING_BIT        0x0002
#define MT6816_BITORDER                     MSBFIRST


static SPISettings MT6816SPISettings(1000000, MT6816_BITORDER, SPI_MODE3);

class MT6816 {
public:
    MT6816(SPISettings settings = MT6816SPISettings, int nCS = -1);
    virtual ~MT6816();

    virtual void init(SPIClass* _spi = &SPI);
    uint16_t readRawAngle();
    bool isNoMagneticReading() {
        return no_magnetic_reading;
    }

private:
    bool parityCheck(uint16_t data);
    uint16_t spi_transfer16(uint16_t outdata);
    SPIClass* spi;
    SPISettings settings;
    bool no_magnetic_reading = false;
    int nCS = -1;
};

#endif /* MT6816_H */
