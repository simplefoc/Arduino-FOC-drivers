
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
    else
        SimpleFOCDebug::println("SS: too many motors");
};


void SettingsStorage::setRegisters(SimpleFOCRegister* registers, int numRegisters){
    this->registers = registers;
    this->numRegisters = numRegisters;
};


void SettingsStorage::init(RegisterIO* comms) {
    this->_io = comms;
    // make sure we have motors and registers
    if (numMotors < 1) {
        SimpleFOCDebug::println("SS: no motors");
        return;
    }
    if (registers==NULL || numRegisters < 1) {
        SimpleFOCDebug::println("SS: no registers");
        return;
    }
};



SettingsStatus SettingsStorage::loadSettings() {
    SimpleFOCDebug::println("Loading settings...");
    beforeLoad();
    uint8_t magic; *_io >> magic;
    if (magic != SIMPLEFOC_SETTINGS_MAGIC_BYTE) {
        SimpleFOCDebug::println("No settings found ");
        return SFOC_SETTINGS_NONE;
    }
    uint8_t rversion; *_io >> rversion;
    if (rversion != SIMPLEFOC_REGISTERS_VERSION) {
        SimpleFOCDebug::println("Registers version mismatch");
        return SFOC_SETTINGS_OLD;
    }
    uint8_t version; *_io >> version;
    if (version != settings_version) {
        SimpleFOCDebug::println("Settings version mismatch");
        return SFOC_SETTINGS_OLD;
    }
    for (int m = 0; m < numMotors; m++) {
        if (numMotors>1)
            SimpleFOCDebug::println("Loading settings for motor ", m);
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
    SimpleFOCDebug::println("Settings loaded");
    return SFOC_SETTINGS_SUCCESS;
};


SettingsStatus SettingsStorage::saveSettings() {
    SimpleFOCDebug::println("Saving settings...");
    beforeSave();
    *_io << (uint8_t)SIMPLEFOC_SETTINGS_MAGIC_BYTE;
    *_io << (uint8_t)SIMPLEFOC_REGISTERS_VERSION;
    *_io << (uint8_t)settings_version;
    for (int m = 0; m < numMotors; m++) {
        if (numMotors>1)
            SimpleFOCDebug::println("Saving settings for motor ", m);
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
    SimpleFOCDebug::println("Settings saved");
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
