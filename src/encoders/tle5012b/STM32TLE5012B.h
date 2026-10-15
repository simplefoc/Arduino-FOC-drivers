#ifndef __TLE5012B_H__
#define __TLE5012B_H__

#include "Arduino.h"

#if defined(_STM32_DEF_)


#define _2PI 6.28318530718f
#define TLE5012B_CPR 32768.0f

#define TLE5012B_READ_REGISTER 0x8000

enum TLE5012B_Register : uint16_t {
    TLE5012B_ANGLE_REG = 0x0020,
    TLE5012B_SPEED_REG = 0x0030
};



class TLE5012B {
    public:
        TLE5012B(int data, int sck, int nCS, uint32_t freq = 1000000);
        ~TLE5012B();
        virtual void init();

	    uint16_t readRawAngle();
	    float getCurrentAngle(); // angle in radians, return current value

        /**
         * Validate the safety word the sensor already returns with every read.
         *
         * The SSC protocol appends a safety word after the data, and this
         * driver already clocks it off the wire, so checking it costs no extra
         * bus traffic. When enabled, a read whose CRC or status bits fail is
         * rejected and the previous good angle is returned instead, rather
         * than passing a corrupted angle up to the motor control loop.
         *
         * Disabled by default so existing behaviour is unchanged.
         */
        void checkSafetyWord(bool enable) { _checkSafety = enable; };

        /** Reads rejected since init: CRC mismatch or a status bit asserting a fault. */
        uint32_t safetyErrorCount() { return _safetyErrors; };

        /** The safety word from the most recent read, for diagnostics. */
        uint16_t lastSafetyWord() { return _lastSafety; };

    private:

        void readBytes(uint16_t reg, uint8_t *data, uint8_t len);
        bool safetyWordValid(uint16_t command, uint16_t value, uint16_t safety);

        bool _checkSafety = false;
        uint32_t _safetyErrors = 0;
        uint16_t _lastSafety = 0;
        uint16_t _lastGoodAngle = 0;

        int _data;
        int _sck;
        int _nCS;
        uint32_t _freq;
        SPI_HandleTypeDef _spi;
};




#endif

#endif