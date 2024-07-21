
#include "MT6816.h"

MT6816::MT6816(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {
};

MT6816::~MT6816() {
};

void MT6816::init(SPIClass* _spi) {
    spi = _spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
        spi->begin();
    }
};

uint16_t MT6816::readRawAngle()  {
    uint16_t angle_data = 0;
    angle_data = spi_transfer16(MT6816_READ_REG_03) << 8;
    angle_data |= spi_transfer16(MT6816_READ_REG_04);

    if ((angle_data & MT6816_NO_MAGNET_WARNING_BIT) == MT6816_NO_MAGNET_WARNING_BIT) {
        this->no_magnetic_reading = true;
    } else {
        this->no_magnetic_reading = false;
    }

    if (!this->parityCheck(angle_data)) {
        return 0;
    }

    return (angle_data >> 2);
}

bool MT6816::parityCheck(uint16_t data) {
    data ^= data >> 8;
    data ^= data >> 4;
    data ^= data >> 2;
    data ^= data >> 1;

    return (~data) & 1;
}

uint16_t MT6816::spi_transfer16(uint16_t outdata) {
    spi->beginTransaction(settings);
    if (nCS>=0)
        digitalWrite(nCS, 0);
    uint16_t result = spi->transfer16(outdata);
    if (nCS>=0)
        digitalWrite(nCS, 1);
    spi->endTransaction();

    return result;
}
