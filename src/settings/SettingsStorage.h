
#pragma once


#include "../comms/SimpleFOCRegisters.h"
#include "BLDCMotor.h"

#define SIMPLEFOC_SETTINGS_MAGIC_BYTE 0x42
#define SIMPLEFOC_SETTINGS_VERSION 0x01

#define SIMPLEFOC_SETTINGS_REGISTERS_MINIMAL { REG_ZERO_ELECTRIC_ANGLE, REG_SENSOR_DIRECTION }


#if !defined(SIMPLEFOC_SETTINGS_MAX_MOTORS)
#define SIMPLEFOC_SETTINGS_MAX_MOTORS 4
#endif


typedef enum : uint8_t  {
    SFOC_SETTINGS_SUCCESS = 0,   // settings loaded/saved successfully
    SFOC_SETTINGS_NONE = 1,      // no settings found to load
    SFOC_SETTINGS_OLD = 2,       // settings found, but version is old
    SFOC_SETTINGS_ERROR = 3      // other error
} SettingsStatus;




class SettingsStorage {
public:
    SettingsStorage();
    ~SettingsStorage();

    void addMotor(BLDCMotor* motor);
    void setRegisters(SimpleFOCRegister* registers, int numRegisters);

    virtual void init(RegisterIO* comms);

    SettingsStatus loadSettings();
    SettingsStatus saveSettings();

    /*
        Change this value if you make changes to the registers saved in code, and have saved settings on existing devices.
        This will cause the device to reset to defaults on the next boot.
    */
    uint8_t settings_version = SIMPLEFOC_SETTINGS_VERSION;

protected:
    virtual void startSaveMotor(uint8_t num);
    virtual void endSaveMotor();
    virtual void startSaveRegister(SimpleFOCRegister reg);
    virtual void endSaveRegister();
    virtual void startLoadMotor(uint8_t num);
    virtual void endLoadMotor();
    virtual void startLoadRegister(SimpleFOCRegister reg);
    virtual void endLoadRegister();

    virtual void beforeLoad();
    virtual void afterLoad();
    virtual void beforeSave();
    virtual void afterSave();

    FOCMotor* motors[SIMPLEFOC_SETTINGS_MAX_MOTORS];
    int numMotors = 0;
    SimpleFOCRegister* registers = NULL;
    int numRegisters = 0;
    RegisterIO* _io;
};
