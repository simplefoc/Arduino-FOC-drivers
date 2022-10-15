
#ifndef MAGNETICSENSOR_MT6816_H
#define MAGNETICSENSOR_MT6816_H

#include "common/base_classes/Sensor.h"
#include "MT6816.h"

class MagneticSensorMT6816 : public Sensor, public MT6816 {
public:
    MagneticSensorMT6816(int nCS = -1, SPISettings settings = MT6816SPISettings);
    virtual ~MagneticSensorMT6816();

    virtual float getSensorAngle() override;
    virtual void init(SPIClass* _spi = &SPI);
};

#endif /* MAGNETICSENSOR_MT6816_H */
