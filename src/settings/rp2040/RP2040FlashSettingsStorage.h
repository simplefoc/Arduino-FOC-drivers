
#pragma once

#if defined(TARGET_RP2040)

#include "../SettingsStorage.h"

class RP2040FlashSettingsStorage : public SettingsStorage, public RegisterIO {
public:
    RP2040FlashSettingsStorage(uint32_t offset = 0x0);
    ~RP2040FlashSettingsStorage();

    void init();

protected:
    RegisterIO& operator<<(float value) override;
    RegisterIO& operator<<(uint32_t value) override;
    RegisterIO& operator<<(uint8_t value) override;
    RegisterIO& operator>>(float& value) override;
    RegisterIO& operator>>(uint32_t& value) override;
    RegisterIO& operator>>(uint8_t& value) override;

    void beforeSave() override;
    void afterSave() override;
    void beforeLoad() override;

    int readBytes(void* valueToSet, int numBytes);
    void reset();

    uint32_t _offset;
    uint8_t* _currptr;
};

#endif