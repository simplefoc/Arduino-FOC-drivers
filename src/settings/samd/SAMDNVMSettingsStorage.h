
#pragma once

/**
 * SAMD can write to their onboard NVM (flash). This class implements the SettingsStorage interface 
 * for storing settings in NVM.
 * 
 * There are two suggested strategies for using the NVM for storing settings:
 * 
 * 1. On chips which have it, use the RWWEE (Read While Write EEPROM) section of the NVM. This is
 *    a small section of the NVM which can be written to while the rest of the NVM is being read.
 *    This is the default strategy used by this class. The size of the RWWEE section varies by chip,
 *    but is at least 2kB. It is seperate from the main flash memory, so doesn't reduce the amount
 *    of flash available for your program.
 * 
 * 2. On chips which don't have a RWWEE section, you can configure a EEPROM section in the NVM. This 
 *    will be at the end of the main NVM, and therefore reduces the flash available for your program.
 * 
 * In either case, you must make sure your board definition file has the correct NVM settings to prevent
 * the configuration from being overwritten by the Arduino IDE / your programmer.
 * 
 * By default, this class will use the RWWEE section at address 0x00400000.
 * To change it, use the build flag SIMPLEFOC_SAMDNVMSETTINGS_BASE to set the base address of your choice.
 * 
 * You can pass an offset to the constructor of this class. This allows you to store multiple sets of 
 * settings in the NVM. The offset should be a multiple of the ROWSIZE (256 bytes).
 * 
 * The memory rows (normally 1 row, but could be more if you save a *lot*) containing settings are always 
 * erased before writing. Don't store other data within the same memory rows.
 * 
 * Don't save to NVM unnecessarily. It has a limited number of write cycles, and writing takes time.
 * 
 */


#include <Arduino.h>

#if defined(_SAMD21_)

#include "../SettingsStorage.h"

#define RWWEE_BASE 0x00400000

#if !defined(SIMPLEFOC_SAMDNVMSETTINGS_BASE)
#define SIMPLEFOC_SAMDNVMSETTINGS_BASE RWWEE_BASE
#endif


class SAMDNVMSettingsStorage : public SettingsStorage, public RegisterIO {
public:
    SAMDNVMSettingsStorage(uint32_t offset = 0x0);
    ~SAMDNVMSettingsStorage();

    void init();

protected:
    RegisterIO& operator<<(float value) override;
    RegisterIO& operator<<(uint32_t value) override;
    RegisterIO& operator<<(uint8_t value) override;
    RegisterIO& operator>>(float& value) override;
    RegisterIO& operator>>(uint32_t& value) override;
    RegisterIO& operator>>(uint8_t& value) override;

    void beforeSave() override;
    void afterSave() override;
    void beforeLoad() override;


    void reset();
    int readBytes(void* valueToSet, int numBytes);
    int writeBytes(void* value, int numBytes);
    void flushPage();

    uint32_t _offset;   // offset into NVRAM to store settings

    uint32_t _ctlB;
    uint8_t* _currptr;     // pointer to current location in NVM
    int _writeIndex;    // index into current page being written
    uint8_t _writeBuffer[NVMCTRL_PAGE_SIZE];  // buffer for writing to NVM
};

#endif
