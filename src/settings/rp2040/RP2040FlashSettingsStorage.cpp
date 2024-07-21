
#include "./RP2040FlashSettingsStorage.h"

#if defined(TARGET_RP2040)

#include "communication/SimpleFOCDebug.h"


RP2040FlashSettingsStorage::RP2040FlashSettingsStorage(uint32_t offset) {
    _offset = offset;
};

RP2040FlashSettingsStorage::~RP2040FlashSettingsStorage() {

};

void RP2040FlashSettingsStorage::init() {
    SettingsStorage::init(this);
    reset();
};


void RP2040FlashSettingsStorage::reset() {
    _currptr = (uint8_t*)_offset;
};



void RP2040FlashSettingsStorage::beforeSave() {
    reset();
};


void RP2040FlashSettingsStorage::afterSave() {

};


void RP2040FlashSettingsStorage::beforeLoad() {
    reset();
};





int RP2040FlashSettingsStorage::readBytes(void* valueToSet, int numBytes) {
    uint8_t* bytes = (uint8_t*)valueToSet;
    for (int i=0;i<numBytes;i++) {
        uint8_t val = *_currptr++;
        bytes[i] = val;
    }
    return numBytes;
}



RegisterIO& RP2040FlashSettingsStorage::operator>>(uint8_t& value) {
    uint8_t val;
    uint8_t num = readBytes(&val, 1);
    if (num==1)
        value = val;
    return *this;
};

RegisterIO& RP2040FlashSettingsStorage::operator>>(float& value) {
    float val;
    uint8_t num = readBytes(&val, 4);
    if (num==4)
        value = val;
    return *this;
};

RegisterIO& RP2040FlashSettingsStorage::operator>>(uint32_t& value) {
    uint32_t val;
    uint8_t num = readBytes(&val, 4);
    if (num==4)
        value = val;
    return *this;
};


RegisterIO& RP2040FlashSettingsStorage::operator<<(uint8_t value) {
    // TODO implement me!
    return *this;
};

RegisterIO& RP2040FlashSettingsStorage::operator<<(float value) {
    // TODO implement me!
    return *this;
};

RegisterIO& RP2040FlashSettingsStorage::operator<<(uint32_t value) {
    // TODO implement me!
    return *this;
};

#endif