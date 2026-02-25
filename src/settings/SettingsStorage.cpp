
#include "./SettingsStorage.h"
#include "communication/SimpleFOCDebug.h"


SettingsStorage::SettingsStorage() {

};


SettingsStorage::~SettingsStorage() {

};



void SettingsStorage::addMotor(BLDCMotor* motor) {
    if (numMotors < SIMPLEFOC_SETTINGS_MAX_MOTORS) {
        motors[numMotors] = motor;
        numMotors++;
    }
#ifndef SIMPLEFOC_DISABLE_DEBUG
    else
        SimpleFOCDebug::println("SS: too many motors");
#endif
};


void SettingsStorage::setRegisters(SimpleFOCRegister* registers, int numRegisters){
    this->registers = registers;
    this->numRegisters = numRegisters;
};


void SettingsStorage::init(RegisterIO* comms) {
    this->_io = comms;
    // make sure we have motors and registers
#ifndef SIMPLEFOC_DISABLE_DEBUG
    if (numMotors < 1) {
        SimpleFOCDebug::println("SS: no motors");
        return;
    }
    if (registers==NULL || numRegisters < 1) {
        SimpleFOCDebug::println("SS: no registers");
        return;
    }
#endif
};



SettingsStatus SettingsStorage::loadSettings() {
#ifndef SIMPLEFOC_DISABLE_DEBUG
    SimpleFOCDebug::println("Loading settings...");
#endif
    beforeLoad();
    uint8_t magic; *_io >> magic;
    if (magic != SIMPLEFOC_SETTINGS_MAGIC_BYTE) {
#ifndef SIMPLEFOC_DISABLE_DEBUG
        SimpleFOCDebug::println("No settings found ");
#endif
        return SFOC_SETTINGS_NONE;
    }
    uint8_t rversion; *_io >> rversion;
    if (rversion != SIMPLEFOC_REGISTERS_VERSION) {
#ifndef SIMPLEFOC_DISABLE_DEBUG
        SimpleFOCDebug::println("Registers version mismatch");
#endif
        return SFOC_SETTINGS_OLD;
    }
    uint8_t version; *_io >> version;
    if (version != settings_version) {
#ifndef SIMPLEFOC_DISABLE_DEBUG
        SimpleFOCDebug::println("Settings version mismatch");
#endif
        return SFOC_SETTINGS_OLD;
    }
    for (int m = 0; m < numMotors; m++) {
#ifndef SIMPLEFOC_DISABLE_DEBUG
        if (numMotors>1)
            SimpleFOCDebug::println("Loading settings for motor ", m);
#endif
        startLoadMotor(m);
        for (int i = 0; i < numRegisters; i++) {
            SimpleFOCRegister reg = registers[i];
            startLoadRegister(reg);
            SimpleFOCRegisters::regs->commsToRegister(*_io, reg, motors[m]);
            endLoadRegister();
        }
        endLoadMotor();
    }
    afterLoad();
#ifndef SIMPLEFOC_DISABLE_DEBUG
    SimpleFOCDebug::println("Settings loaded");
#endif
    return SFOC_SETTINGS_SUCCESS;
};


SettingsStatus SettingsStorage::saveSettings() {
#ifndef SIMPLEFOC_DISABLE_DEBUG
    SimpleFOCDebug::println("Saving settings...");
#endif
    beforeSave();
    *_io << (uint8_t)SIMPLEFOC_SETTINGS_MAGIC_BYTE;
    *_io << (uint8_t)SIMPLEFOC_REGISTERS_VERSION;
    *_io << (uint8_t)settings_version;
    for (int m = 0; m < numMotors; m++) {
        if (numMotors>1)
#ifndef SIMPLEFOC_DISABLE_DEBUG
            SimpleFOCDebug::println("Saving settings for motor ", m);
#endif
        startSaveMotor(m);
        for (int i = 0; i < numRegisters; i++) {
            SimpleFOCRegister reg = registers[i];
            startSaveRegister(reg);
            SimpleFOCRegisters::regs->registerToComms(*_io, reg, motors[m]);
            endSaveRegister();
        }
        endSaveMotor();
    }
    afterSave();
#ifndef SIMPLEFOC_DISABLE_DEBUG
    SimpleFOCDebug::println("Settings saved");
#endif
    return SFOC_SETTINGS_SUCCESS;
};

// empty implementation for these
    
void SettingsStorage::startSaveMotor(uint8_t num) {};
void SettingsStorage::endSaveMotor() {};
void SettingsStorage::startSaveRegister(SimpleFOCRegister reg) {};
void SettingsStorage::endSaveRegister() {};
void SettingsStorage::startLoadMotor(uint8_t num) {};
void SettingsStorage::endLoadMotor() {};
void SettingsStorage::startLoadRegister(SimpleFOCRegister reg) {};
void SettingsStorage::endLoadRegister() {};

void SettingsStorage::beforeLoad() {};
void SettingsStorage::afterLoad() {};
void SettingsStorage::beforeSave() {};
void SettingsStorage::afterSave() {};
