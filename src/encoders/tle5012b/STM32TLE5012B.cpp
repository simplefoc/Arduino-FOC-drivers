
#include "STM32TLE5012B.h"

#if defined(_STM32_DEF_)


#include "utility/spi_com.h"
extern "C" uint32_t spi_getClkFreqInst(SPI_TypeDef *spi_inst);



TLE5012B::TLE5012B(int data, int sck, int nCS, uint32_t freq) {
    _data = data;
    _sck = sck;
    _nCS = nCS;
    _freq = freq;
};

TLE5012B::~TLE5012B() {

};


void TLE5012B::init() {
    pinMode(_nCS, OUTPUT);
    digitalWrite(_nCS, HIGH);

    // initialize pins
    GPIO_InitTypeDef gpio;
    gpio.Pin = digitalPinToBitMask(_data);
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(digitalPinToPort(_data), &gpio);
    gpio.Pin = digitalPinToBitMask(_sck);
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(digitalPinToPort(_sck), &gpio);

    SPI_TypeDef *spi_data = (SPI_TypeDef*)pinmap_peripheral(digitalPinToPinName(_data), PinMap_SPI_MOSI);
    SPI_TypeDef *spi_sclk = (SPI_TypeDef*)pinmap_peripheral(digitalPinToPinName(_sck), PinMap_SPI_SCLK);
    SPI_TypeDef *spi_inst = (SPI_TypeDef*)pinmap_merge_peripheral(spi_data, spi_sclk);

    pinmap_pinout(digitalPinToPinName(_data), PinMap_SPI_MOSI);
    pinmap_pinout(digitalPinToPinName(_sck), PinMap_SPI_SCLK);

    #if defined SPI1_BASE
    if (spi_inst == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();
    }
    #endif

    #if defined SPI2_BASE
    if (spi_inst == SPI2) {
        __HAL_RCC_SPI2_CLK_ENABLE();
        __HAL_RCC_SPI2_FORCE_RESET();
        __HAL_RCC_SPI2_RELEASE_RESET();
    }
    #endif

    #if defined SPI3_BASE
    if (spi_inst == SPI3) {
        __HAL_RCC_SPI3_CLK_ENABLE();
        __HAL_RCC_SPI3_FORCE_RESET();
        __HAL_RCC_SPI3_RELEASE_RESET();
    }
    #endif

    #if defined SPI4_BASE
    if (spi_inst == SPI4) {
        __HAL_RCC_SPI4_CLK_ENABLE();
        __HAL_RCC_SPI4_FORCE_RESET();
        __HAL_RCC_SPI4_RELEASE_RESET();
    }
    #endif

    #if defined SPI5_BASE
    if (spi_inst == SPI5) {
        __HAL_RCC_SPI5_CLK_ENABLE();
        __HAL_RCC_SPI5_FORCE_RESET();
        __HAL_RCC_SPI5_RELEASE_RESET();
    }
    #endif

    #if defined SPI6_BASE
    if (spi_inst == SPI6) {
        __HAL_RCC_SPI6_CLK_ENABLE();
        __HAL_RCC_SPI6_FORCE_RESET();
        __HAL_RCC_SPI6_RELEASE_RESET();
    }
    #endif

    _spi.Instance = spi_inst;
    _spi.Init.Direction = SPI_DIRECTION_1LINE;
    _spi.Init.Mode = SPI_MODE_MASTER;
    _spi.Init.DataSize = SPI_DATASIZE_8BIT;
    _spi.Init.CLKPolarity = SPI_POLARITY_LOW;
    _spi.Init.CLKPhase = SPI_PHASE_2EDGE;
    _spi.Init.NSS = SPI_NSS_SOFT;
    _spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    _spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    _spi.Init.CRCPolynomial = 7;
    _spi.Init.TIMode = SPI_TIMODE_DISABLE;
    #if defined(SPI_NSS_PULSE_DISABLE)
    _spi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    #endif

    uint32_t spi_freq = spi_getClkFreqInst(spi_inst);
    if (_freq >= (spi_freq / SPI_SPEED_CLOCK_DIV2_MHZ)) {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    } else if (_freq >= (spi_freq / SPI_SPEED_CLOCK_DIV4_MHZ)) {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    } else if (_freq >= (spi_freq / SPI_SPEED_CLOCK_DIV8_MHZ)) {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    } else if (_freq >= (spi_freq / SPI_SPEED_CLOCK_DIV16_MHZ)) {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    } else if (_freq >= (spi_freq / SPI_SPEED_CLOCK_DIV32_MHZ)) {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    } else if (_freq >= (spi_freq / SPI_SPEED_CLOCK_DIV64_MHZ)) {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    } else if (_freq >= (spi_freq / SPI_SPEED_CLOCK_DIV128_MHZ)) {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    } else {
        _spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    }

    if (HAL_SPI_Init(&_spi) != HAL_OK) {
        // setup error
        Serial.println("TLE5012B setup error");
    }
};

uint16_t TLE5012B::readRawAngle() {
    uint8_t data[4];
    readBytes(TLE5012B_ANGLE_REG, data, 2);
    return (((uint16_t)data[0] << 8) | data[1]) & 0x7FFF;
}; 


float TLE5012B::getCurrentAngle() {
    return ((float)readRawAngle())/TLE5012B_CPR * _2PI;
}; // angle in radians, return current value


void TLE5012B::readBytes(uint16_t reg, uint8_t *data, uint8_t len) {
    digitalWrite(_nCS, LOW);

    reg |= TLE5012B_READ_REGISTER + (len>>1);
    uint8_t txbuffer[2] = { (uint8_t)(reg >> 8), (uint8_t)(reg & 0x00FF) };    
    HAL_SPI_Transmit(&_spi, txbuffer, 2, 100); // TODO check return value for error, timeout
    //delayMicroseconds(1);
    HAL_SPI_Receive(&_spi, data, len + 2, 100);

    digitalWrite(_nCS, HIGH);
};


#endif
