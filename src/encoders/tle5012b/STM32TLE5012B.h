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
    private:

        void readBytes(uint16_t reg, uint8_t *data, uint8_t len);

        int _data;
        int _sck;
        int _nCS;
        uint32_t _freq;
        SPI_HandleTypeDef _spi;
};




#endif

#endif