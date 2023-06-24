
# I2C memories settings drivers

Store settings to I2C EEPROMs.

## CAT24C64 I2C EEPROMs

:warning: not yet finished or tested!

Datasheet describes the CAT24C64 as: _The CAT24C64 is a 64 Kb CMOS Serial EEPROM device, internally organized as 8192 words of 8 bits each._

Provide I2C address in constructor:

`CAT24I2CFlashSettingsStorage settings = CAT24I2CFlashSettingsStorage(0xA0);`

If you want to use a different I2C bus than the default, you can pass it to the constructor:

`settings.init(&Wire2);`

You can use multiple settings objects in the same EEPROM, by providing an offset to the constructor:

```c++
CAT24I2CFlashSettingsStorage settings = CAT24I2CFlashSettingsStorage(0xA0);
CAT24I2CFlashSettingsStorage settings = CAT24I2CFlashSettingsStorage(0xA0, 80);
CAT24I2CFlashSettingsStorage settings = CAT24I2CFlashSettingsStorage(0xA0, 160);
```
