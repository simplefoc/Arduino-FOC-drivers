# SAMD21 settings driver

Driver for storing settings on SAMD21 chips.

There are two options:

1. **RWW Flash** \
Some SAMD21 chips have a seperate RWW flash area, of 2kB-8kB size. This area is seperate from the main flash array. \
Note: this method is implemented and tested.

2. **EEPROM emulation** \
All SAMD21s support EEPROM emulation, where an area of the main flash is reserved for the application storage. \
:warning: This method is not yet implemented fully, and untested.

## Using RWW flash

To use the RWW flash to store settings, you have to make sure the flash region is not locked (you can set the lock fuses using MCP Studio or other SAM development tools).

Otherwise there is nothing special to do. When reprogramming the chip, be careful not to erase the RWW area.

You can use multiple settings objects to store to different areas of the RWW. 
To do so, specify an offset to the constructor:

```c++
SAMDNVMSettingsStorage settings0 = SAMDNVMSettingsStorage();
SAMDNVMSettingsStorage settings1 = SAMDNVMSettingsStorage(NVMCTRL_ROW_SIZE);
SAMDNVMSettingsStorage settings2 = SAMDNVMSettingsStorage(NVMCTRL_ROW_SIZE*2);
```

Note: the offset must be a multiple of the flash memory row size, typically 256 bytes.


## Using EEPROM emulation

TODO implement and test this method

To use the EEPROM emulation, use a tool like MCP Studio to set the chip fuses to reserve an area of flash for EEPROM emulation:

![](eeprom_emulation_screenshot.jpg)

Normally 256 bytes (one flash page) should be plenty for SimpleFOC settings... Obviously, the area you reserve for EEPROM can't be used for your main program. 

Add a build-flag to your build to set the base address of your EEPROM memory:

`-DSIMPLEFOC_SAMDNVMSETTINGS_BASE=0x003FF800`

Then use the settings as normal.

You'll have to adjust your board files to exclude the EEPROM area in the ldscript to prevent it being erased when you re-program, if you care to keep your settings when reflashing.
