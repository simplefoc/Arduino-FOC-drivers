# SimpleFOC Settings Storage

Code to persist calibration data and motor settings to and from storage. The SettingsStorage is abstract, and different implementations exist for different concete storage media.

The SettingsStorage is based on the SimpleFOCRegisters abstraction, so this means you can load or store any of the registers (representing SimpleFOC settings) that is both readable and writeable (e.g. while the current shaft velocity is available as a register, you can't store it as a setting because its read-only).

The SettingsStorage can save the settings for one or more motors, and you can customize which settings are saved. If you don't customize, only the motor
calibration will be stored (REG_ZERO_ELECTRIC_ANGLE, REG_SENSOR_DIRECTION).

## Hardware requirements

Obviously, to save settings you need a place to store them where they won't be lost when you power down. There can be many solutions to this question, and we attempt to provide some simple drivers for the common storage solutions people might want.

But its also quite easy to extend the system to support your specific needs.

Supported hardware:

- [SAMD NVM](samd/) \
Some SAMD chips have a built-in NVRAM flash, which can be written from user code.
- [CAT24 I2C Flash EEPROMs](i2c/) :warning: UNFINISHED \
Simple programmable memories with I2C interface. Memory space is quite limited, but enough for storing settings.
- [STM32G4 Flash](stm32/) \
Store settings directly to STM32G4 MCU's on-board flash.

See Roadmap, below, for systems we may support in the future.


## Basic Usage

Using SettingsStorage is simple. Just add a `settings.loadSettings()` after setting up your defaults, but before calling init() on the objects. And call `saveSettings()` after initialization of the motor has (successfully) completed.

This example saves just the motor calibration, but you might want to add additional code to save other registers, or make your solution more user-friendly. See the other examples below.

```c++

#include "settings/samd/SAMDNVMSettingsStorage.h"

SAMDNVMSettingsStorage settings = SAMDNVMSettingsStorage();
BLDCMotor motor = BLDCMotor(...);

void setup() {
    SimpleFOCDebug::enable(); // show debug messages from settings

    // initialize the settings early in the setup
    settings.addMotor(&motor);
    settings.init();

    // first set your defaults
    driver.voltage_power_supply = 12.0f;
    driver.voltage_limit = 12.0f; // 12V driver limit, like PSU
    motor.voltage_limit = 6.0f; // 6V motor limit

    // then try to load the settings
    SettingsStatus loadStatus = settings.loadSettings();

    // driver init code, etc...
    ...

    // then init the motor
    motor.init();
    motor.initFOC();

    // and if the settings were not loaded earlier, then save them
    if (motor.motor_status == FOCMotorStatus::motor_ready && loadStatus != SFOC_SETTINGS_SUCCESS) {
        settings.saveSettings();
    }

    // any other initialization
    ...
}

```

### Choosing registers

By default, only the calibration data (motor native direction, electrical zero position) are saved. To save additional registers, you can choose them before calling `SettingsStorage.init()`, as in the example below:

```c++

SimpleFOCRegister settingsRegisters[] = { REG_ZERO_ELECTRIC_ANGLE, REG_SENSOR_DIRECTION, REG_VEL_PID_P, REG_VEL_PID_I, REG_VEL_PID_D, REG_VEL_LPF_T, REG_VEL_LIMIT, REG_VEL_MAX_RAMP, REG_VOLTAGE_LIMIT, REG_MOTION_DOWNSAMPLE, REG_CONTROL_MODE, REG_TORQUE_MODE, REG_PHASE_RESISTANCE, REG_KV, REG_INDUCTANCE };
CAT24I2CFlashSettingsStorage settings = CAT24I2CFlashSettingsStorage(0xA0);
BLDCMotor motor = BLDCMotor(...);

void setup() {
    SimpleFOCDebug::enable(); // show debug messages from settings

    // initialize the settings early in the setup
    settings.setRegisters(mySettingsRegisters, sizeof(mySettingsRegisters));
    settings.addMotor(&motor);
    settings.init();
    ...
}
```



### Using settings with commander

You can easily interact with the settings via the commander, for example to save your PID tuning values:

```c++

SimpleFOCRegister settingsRegisters[] = { REG_ZERO_ELECTRIC_ANGLE, REG_SENSOR_DIRECTION, REG_VEL_PID_P, REG_VEL_PID_I, REG_VEL_PID_D, REG_VEL_LPF_T };

CAT24I2CFlashSettingsStorage settings = CAT24I2CFlashSettingsStorage(0xA0);

Commander commander;

void doSaveSettings(char *cmd) { settings.saveSettings(); }
void doLoadSettings(char *cmd) { settings.loadSettings(); }


void setup() {
    ...

    settings.setRegisters(settingsRegisters, sizeof(settingsRegisters));
    settings.addMotor(&motor);
    settings.init();

    ...

    commander.addCommand('s', doSaveSettings, "Save settings");
    commander.addCommand('l', doLoadSettings, "Load settings");

    ...

}


void run() {
    motor.move();
    motor.loopFOC();
    commander.run();
}
```

## Roadmap

The following ideas for storage backends are on the roadmap, with no definate schedule for implementation:

- ESP32 SPIFFs Filesystem
- ESP32 Preferences
- Arduino EEPROM library
- In-Memory Buffer (for use with other libraries or abstractions)
- JSON String (for printing to console)
