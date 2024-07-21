
#pragma once

#include "../SettingsStorage.h"
#include "Wire.h"

class CAT24I2CFlashSettingsStorage : public SettingsStorage, public RegisterIO {
public:
    CAT24I2CFlashSettingsStorage(uint8_t address = 0xA0, uint16_t offset = 0x0);
    ~CAT24I2CFlashSettingsStorage();

    void init(TwoWire* wire = &Wire);

protected:
    RegisterIO& operator<<(float value) override;
    RegisterIO& operator<<(uint32_t value) override;
    RegisterIO& operator<<(uint8_t value) override;
    RegisterIO& operator>>(float& value) override;
    RegisterIO& operator>>(uint32_t& value) override;
    RegisterIO& operator>>(uint8_t& value) override;

    void beforeSave() override;
    void beforeLoad() override;

    void reset();
    int readBytes(void* valueToSet, int numBytes);
    int writeBytes(void* value, int numBytes);

    TwoWire* _wire;
    uint8_t _address;           // i2c address
    uint16_t _offset;           // offset into NVRAM to store settings
    uint16_t _currptr = 0;      // pointer to current location in NVRAM

};
