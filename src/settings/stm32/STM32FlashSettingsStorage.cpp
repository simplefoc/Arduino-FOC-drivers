

#include "./STM32FlashSettingsStorage.h"

#if defined(STM32G4xx)

#include "communication/SimpleFOCDebug.h"


STM32FlashSettingsStorage::STM32FlashSettingsStorage(uint32_t address) {
    _address = address;
};

STM32FlashSettingsStorage::~STM32FlashSettingsStorage(){};


void STM32FlashSettingsStorage::init(){
    if (!IS_FLASH_PROGRAM_ADDRESS(_address))
        SimpleFOCDebug::println("SS: Invalid address");
    SettingsStorage::init(this);
    reset();
};


void STM32FlashSettingsStorage::reset() {
    _currptr = (uint8_t*)_address;
};


void STM32FlashSettingsStorage::beforeLoad(){
    reset();
};

void STM32FlashSettingsStorage::beforeSave(){
    _writeptr = (uint8_t*)_address;
    _buffed = 0;
    _writeBuffer.l = 0;
    _page = PAGE_OF(_writeptr);
    if (HAL_FLASH_Unlock()!=HAL_OK)
        SimpleFOCDebug::println("SS: Flash unlock err");
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
    erasePage(_page);
};


void STM32FlashSettingsStorage::erasePage(uint32_t page) {
    FLASH_EraseInitTypeDef eraseInit;    
    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.Page = page;
    eraseInit.Banks = 0;//_bank;
    eraseInit.NbPages = 1;
    uint32_t err;
    SimpleFOCDebug::print("SS: erase page ");
    SimpleFOCDebug::println((int)page);
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    if (HAL_FLASHEx_Erase(&eraseInit, &err) != HAL_OK) {
        uint32_t ferr = HAL_FLASH_GetError();
        SimpleFOCDebug::print("SS: flash erase err nr ");
        SimpleFOCDebug::println((int)ferr);
        HAL_FLASH_Lock();
        return;
    }
};


void STM32FlashSettingsStorage::flushBuffer() {
    uint32_t page = PAGE_OF(_writeptr+7);
    if (page != _page) {
        erasePage(page);
        _page = page;
    }
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)_writeptr, _writeBuffer.l)!=HAL_OK) {
        uint32_t ferr = HAL_FLASH_GetError();
        SimpleFOCDebug::println("SS: flash write err nr ");
        SimpleFOCDebug::println((int)ferr);
    }
    _writeptr += 8;
    _buffed = 0;
    _writeBuffer.l = 0;
};


void STM32FlashSettingsStorage::afterSave(){
    if (_buffed > 0)
        flushBuffer();
    HAL_FLASH_Lock();
};



int STM32FlashSettingsStorage::writeBytes(void* value, int numBytes) {
    uint8_t* bytes = (uint8_t*)value;
    for (int i=0;i<numBytes;i++) {
        _writeBuffer.b[_buffed++] = bytes[i];
        if (_buffed == 8)
            flushBuffer();
    }
    return numBytes;
}



int STM32FlashSettingsStorage::readBytes(void* valueToSet, int numBytes) {
    uint8_t* bytes = (uint8_t*)valueToSet;
    for (int i=0;i<numBytes;i++) {
        uint8_t val = *_currptr++;
        bytes[i] = val;
    }
    return numBytes;
}


RegisterIO& STM32FlashSettingsStorage::operator>>(uint8_t& value) {
    uint8_t val;
    uint8_t num = readBytes(&val, 1);
    if (num==1)
        value = val;
    return *this;
};

RegisterIO& STM32FlashSettingsStorage::operator>>(float& value) {
    float val;
    uint8_t num = readBytes(&val, 4);
    if (num==4)
        value = val;
    return *this;
};

RegisterIO& STM32FlashSettingsStorage::operator>>(uint32_t& value) {
    uint32_t val;
    uint8_t num = readBytes(&val, 4);
    if (num==4)
        value = val;
    return *this;
};



RegisterIO& STM32FlashSettingsStorage::operator<<(uint8_t value) {
    writeBytes(&value, 1);
    return *this;
};


RegisterIO& STM32FlashSettingsStorage::operator<<(float value) {
    writeBytes(&value, 4);
    return *this;
};


RegisterIO& STM32FlashSettingsStorage::operator<<(uint32_t value) {
    writeBytes(&value, 4);
    return *this;
};

#endif