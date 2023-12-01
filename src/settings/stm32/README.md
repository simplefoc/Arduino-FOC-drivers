
# STM32 Flash Settings Storage

Store your settings directly to STM32 MCU's on-board flash memory.

To store the settings, the flash memory has to be erased in whole pages. 

The page size is typically 1kB or 2kB. You can check your page size in the datasheet for your MCU.

The default settings storage location is the last page of the flash memory, meaning you have one page worth
of space, maximum. If you need more space than this, you can provide the start address for the settings storage.

Needless to say, you can only store settings to flash if that memory is not also needed by your program. So you have to have some reserve space in the flash of the MCU you're using. If memory is too tight, use a STM32 model with more flash.

## Compatibility

Currently only **STM32G4** MCUs are supported.

G4s are nice MCUs for using the flash because the page size is 2kB and you can erase the flash page by page. For this reason the default settings use the last page of flash as the storage location, leaving all the rest of the flash for your code and data. Pages are 2kB in size, which should be more than enough for most setups.

Note: systems with dual bank flash have not been tested, and will probably not work.

## Board setup

When storing settings to the Flash of your STM32 MCU, you have to consider what happens when you reprogram the MCU. Unless you have taken steps in the board setup files to reserve the pages of flash used by the settings, they will generally be erased and/or overwritten when programming the MCU.

To solve this, you can configure your board setup to reserve memory for settings, and not use that memory for program storage. Configuring STM32 board definitions for Arduino is out of scope for this readme, please find another reference.

## Usage

Using it is easy. Watch out if you have more than one page worth of settings to store, in which case you will need to configure the address to a different value from the default one.

```c++
// uses default address, which is 1 page below the maximum flash address, e.g. the last page of the flash
STM32FlashSettingsStorage settings = STM32FlashSettingsStorage();

// or use an address of your choice, in this case 10kB below the end of flash:
#define SETTINGS_ADDRESS (FLASH_BASE + FLASH_SIZE - (1024*10))
STM32FlashSettingsStorage settings = STM32FlashSettingsStorage(SETTINGS_ADDRESS);

// use as normal in the setup method:
void setup() {
    SimpleFOCDebug::enable();

    // link driver etc
    // set default settings on motor, etc

    settings.addMotor(&motor);
    settings.init();
    SettingsStatus loadStatus = settings.loadSettings();
    driver.init();
    motor.init();
    motor.initFOC();
    if (motor.motor_status == FOCMotorStatus::motor_ready && loadStatus != SFOC_SETTINGS_SUCCESS) {
        settings.saveSettings();
    }    
}
```