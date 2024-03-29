
#include "./CAT24I2CFlashSettingsStorage.h"


CAT24I2CFlashSettingsStorage::CAT24I2CFlashSettingsStorage(uint8_t address, uint16_t offset) {
    _address = address;
    _offset = offset;
};



CAT24I2CFlashSettingsStorage::~CAT24I2CFlashSettingsStorage() {};



void CAT24I2CFlashSettingsStorage::init(TwoWire* wire) {
    SettingsStorage::init(this);
    _wire = wire;
    reset();
};



RegisterIO& CAT24I2CFlashSettingsStorage::operator>>(uint8_t& valueToSet) {
    readBytes(&valueToSet, 1);
    return *this;
};



RegisterIO& CAT24I2CFlashSettingsStorage::operator>>(float& valueToSet) {
    readBytes(&valueToSet, 4);
    return *this;
};



RegisterIO& CAT24I2CFlashSettingsStorage::operator>>(uint32_t& valueToSet) {
    readBytes(&valueToSet, 4);
    return *this;
};



RegisterIO& CAT24I2CFlashSettingsStorage::operator<<(uint8_t value) {
    writeBytes(&value, 1);
    return *this;
};



RegisterIO& CAT24I2CFlashSettingsStorage::operator<<(float value) {
    writeBytes(&value, 4);
    return *this;
};



RegisterIO& CAT24I2CFlashSettingsStorage::operator<<(uint32_t value) {
    writeBytes(&value, 4);
    return *this;
};



void CAT24I2CFlashSettingsStorage::beforeSave() {
    reset();
};


void CAT24I2CFlashSettingsStorage::beforeLoad() {
    reset();
    _wire->beginTransmission(_address);
    _wire->write(_currptr >> 8);
    _wire->write(_currptr & 0xFF);
    _wire->endTransmission(false);
};



void CAT24I2CFlashSettingsStorage::reset() {
    _currptr = _offset;
};


int CAT24I2CFlashSettingsStorage::readBytes(void* valueToSet, int numBytes) {
    int read = _wire->requestFrom((uint8_t)_address, (uint8_t)numBytes, (uint8_t)true);
    _currptr += read;
    if (read==numBytes)
        _wire->readBytes((uint8_t*)valueToSet, numBytes);
    return read;
};


int CAT24I2CFlashSettingsStorage::writeBytes(void* value, int numBytes) {
    _wire->beginTransmission(_address);
    _wire->write(_currptr >> 8);
    _wire->write(_currptr & 0xFF);
    size_t written = _wire->write((uint8_t*)value, numBytes);
    _wire->endTransmission(true);
    _currptr += written;
    delay(5); // write-cycle time
    return written;
};
