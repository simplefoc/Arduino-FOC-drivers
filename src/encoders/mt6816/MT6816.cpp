
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
    uint8_t data[3] = {0};

    data[0] = 0x83; // command to read MSB
    data[1] = 0;
    data[2] = 0;

    spi->beginTransaction(settings);
    if (nCS>=0)
        digitalWrite(nCS, 0);

    spi->transfer(data, 3);

    if (nCS>=0)
        digitalWrite(nCS, 1);
    spi->endTransaction();

    angle_data = (data[1] & 0xFF) << 8;
    angle_data |= (data[2] & 0xFF);

    if ((angle_data & MT6816_NO_MAGNET_WARNING_BIT) == MT6816_NO_MAGNET_WARNING_BIT) {
        this->no_magnetic_reading = true;
    } else {
        this->no_magnetic_reading = false;
    }

    if (!this->parityCheck(angle_data)) {
        return 0;
    }

    return (angle_data >> 2) & 0x3FFF; // 14 bits of angle data
}

bool MT6816::parityCheck(uint16_t data) {
    data ^= data >> 8;
    data ^= data >> 4;
    data ^= data >> 2;
    data ^= data >> 1;

    return (~data) & 1;
}
