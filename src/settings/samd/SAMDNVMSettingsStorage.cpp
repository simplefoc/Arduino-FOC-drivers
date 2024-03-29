


#include "./SAMDNVMSettingsStorage.h"


#if defined(_SAMD21_)

#include "communication/SimpleFOCDebug.h"


SAMDNVMSettingsStorage::SAMDNVMSettingsStorage(uint32_t offset) {
    _offset = offset;
};

SAMDNVMSettingsStorage::~SAMDNVMSettingsStorage(){};

void SAMDNVMSettingsStorage::init(){
    SettingsStorage::init(this);
    reset();
};



void SAMDNVMSettingsStorage::reset(){
    _currptr = (uint8_t*) SIMPLEFOC_SAMDNVMSETTINGS_BASE + _offset; // add offset to NVM base address
};



void SAMDNVMSettingsStorage::beforeLoad(){
    reset();
};


void SAMDNVMSettingsStorage::beforeSave(){
    reset();
    _writeIndex = 0;

    // set manual write mode
    _ctlB = NVMCTRL->CTRLB.reg;
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_MANW;
    while (NVMCTRL->INTFLAG.bit.READY == 0) {};
    // unlock region - user can do it with fuses
    // ret_status_code = nvm_execute_command(NVM_COMMAND_UNLOCK_REGION, SIMPLEFOC_SAMDNVMSETTINGS_BASE, 0);
    // if (ret_status_code != STATUS_OK)
    //     SimpleFOCDebug::println("NVM unlock error ", ret_status_code);
    // SimpleFOCDebug::println("NVM lock: ", NVMCTRL->LOCK.reg);
    // erase rows 
    // TODO handle case that it is more than one row
    uint8_t cmd;
    if (SIMPLEFOC_SAMDNVMSETTINGS_BASE >= RWWEE_BASE) {
        cmd = 0x1A;
        NVMCTRL->ADDR.reg = ((uint32_t)_currptr - SIMPLEFOC_SAMDNVMSETTINGS_BASE) / 2;
    }
    else {
        cmd = NVMCTRL_CTRLA_CMD_ER;
        NVMCTRL->ADDR.reg = ((uint32_t)_currptr) / 2;
    }
    NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;
    NVMCTRL->INTFLAG.bit.ERROR = 0;
    NVMCTRL->CTRLA.reg = cmd | NVMCTRL_CTRLA_CMDEX_KEY;
    delay(1);
    while (NVMCTRL->INTFLAG.bit.READY == 0) {};
    if (NVMCTRL->INTFLAG.bit.ERROR == 1)
        SimpleFOCDebug::println("NVM erase error ", NVMCTRL->STATUS.reg);
    else
        SimpleFOCDebug::println("NVM erased row @", (int)_currptr);
};


void SAMDNVMSettingsStorage::afterSave() {
    if (_writeIndex>0)
        flushPage();
    // restore ctlb
    delay(1);
    NVMCTRL->CTRLB.reg =_ctlB;
    while (NVMCTRL->INTFLAG.bit.READY == 0) {};
}


void SAMDNVMSettingsStorage::flushPage(){
    // if we have an odd number of bytes, pad with 0xFF
    if (_writeIndex%2==1) {
        _writeBuffer[_writeIndex++] = 0xFF;
    }
    // erase page buffer
    // NVMCTRL->ADDR.reg = ((uint32_t)_currptr - SIMPLEFOC_SAMDNVMSETTINGS_BASE) / 2;
    // NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;
    // while (NVMCTRL->INTFLAG.bit.READY == 0) {};
    // copy writeBuffer to NVM, using 16-bit writes
    uint16_t* src = (uint16_t*)_writeBuffer;
    uint16_t* dst = (uint16_t*)_currptr;
    for (int i=0;i<_writeIndex/2;i++) {
        *dst++ = *src++;
    }
    // write page
    uint8_t cmd;
    if (SIMPLEFOC_SAMDNVMSETTINGS_BASE >= RWWEE_BASE) {
        cmd = 0x1C;
        NVMCTRL->ADDR.reg = ((uint32_t)_currptr - SIMPLEFOC_SAMDNVMSETTINGS_BASE) / 2;
    }
    else {
        cmd = NVMCTRL_CTRLA_CMD_WP;
        NVMCTRL->ADDR.reg = ((uint32_t)_currptr) / 2;
    }
    NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;
    NVMCTRL->INTFLAG.bit.ERROR = 0;
    NVMCTRL->CTRLA.reg = cmd | NVMCTRL_CTRLA_CMDEX_KEY;
    delay(1);
    while (NVMCTRL->INTFLAG.bit.READY == 0) {};
    if (NVMCTRL->INTFLAG.bit.ERROR == 1)
        SimpleFOCDebug::println("NVM write error ", NVMCTRL->STATUS.reg);
    else
        SimpleFOCDebug::println("NVM wrote page @", (int)_currptr);
    // reset writeBuffer pointer and advance currptr to next page
    _writeIndex = 0;
    _currptr+=NVMCTRL_PAGE_SIZE;
}


int SAMDNVMSettingsStorage::readBytes(void* valueToSet, int numBytes) {
    uint8_t* bytes = (uint8_t*)valueToSet;
    for (int i=0;i<numBytes;i++) {
        uint8_t val = *_currptr++;
        bytes[i] = val;
    }
    return numBytes;
}

int SAMDNVMSettingsStorage::writeBytes(void* value, int numBytes) {
    uint8_t* bytes = (uint8_t*)value;
    for (int i=0;i<numBytes;i++) {
        _writeBuffer[_writeIndex] = *bytes++;
        _writeIndex++;
        if (_writeIndex==NVMCTRL_PAGE_SIZE)
            flushPage();
    }
    return numBytes;
}




RegisterIO& SAMDNVMSettingsStorage::operator>>(uint8_t& value){
    uint8_t val;
    uint8_t num = readBytes(&val, 1);
    if (num==1)
        value = val;
    return *this;
};


RegisterIO& SAMDNVMSettingsStorage::operator>>(float& value){
    float val;
    uint8_t num = readBytes(&val, 4);
    if (num==4)
        value = val;
    return *this;
};


RegisterIO& SAMDNVMSettingsStorage::operator>>(uint32_t& value){
    uint32_t val;
    uint8_t num = readBytes(&val, 4);
    if (num==4)
        value = val;
    return *this;
};

RegisterIO& SAMDNVMSettingsStorage::operator<<(uint8_t value){
    writeBytes(&value, 1);
    return *this;
};


RegisterIO& SAMDNVMSettingsStorage::operator<<(float value){
    writeBytes(&value, 4);
    return *this;
};


RegisterIO& SAMDNVMSettingsStorage::operator<<(uint32_t value){
    writeBytes(&value, 4);
    return *this;
};

#endif
