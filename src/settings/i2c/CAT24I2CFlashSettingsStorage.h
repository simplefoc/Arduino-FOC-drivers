
#pragma once

#include "../SettingsStorage.h"
#include "Wire.h"

class CAT24I2CFlashSettingsStorage : public SettingsStorage {
public:
    CAT24I2CFlashSettingsStorage(uint8_t address = 0xA0, uint16_t offset = 0x0);
    ~CAT24I2CFlashSettingsStorage();

    void init(TwoWire* wire = &Wire);

protected:
    uint8_t readByte(uint8_t* valueToSet) override;
    uint8_t readFloat(float* valueToSet) override;
    uint8_t readInt(uint32_t* valueToSet) override;

    uint8_t writeByte(uint8_t value) override;
    uint8_t writeFloat(float value) override;
    uint8_t writeInt(uint32_t value) override;

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
