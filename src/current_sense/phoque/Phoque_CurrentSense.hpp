#pragma once

#if defined(ARDUINO_PHOQUE)

#include "common/base_classes/CurrentSense.h"

class Phoque_CurrentSense : public CurrentSense
{
protected:
	volatile uint16_t *adc1_buffer = nullptr;
	volatile uint16_t *adc2_buffer = nullptr;
	bool read_bemf;
public:
	Phoque_CurrentSense(float shunt_resistor, float gain, bool read_bemf=false);
	Phoque_CurrentSense(float mVpA, bool read_bemf=false);
	virtual ~Phoque_CurrentSense();

	virtual int init() override;
	virtual PhaseCurrent_s getPhaseCurrents() override;
	virtual int driverAlign(float align_voltage, bool modulation_centered = false) override;

	virtual float readMillivolt(const int pin);
	virtual uint16_t readRaw(const int pin) = 0;

protected:
	void GPIO_Init();
	void DMA_Init();
	virtual void OPAMP_Init();
	//Configure ADC1, child classes should configure each rank to sample, return number of samples
	virtual int ADC1_Init(ADC_HandleTypeDef* hadc1);
	//Configure ADC2, child classes should configure each rank to sample, return number of samples
	virtual int ADC2_Init(ADC_HandleTypeDef* hadc2);
	virtual void DMA1_Init(ADC_HandleTypeDef *hadc, DMA_HandleTypeDef *hdma_adc, DMA_Channel_TypeDef* channel, uint32_t request);
	virtual void* SyncLowSide(void* driver_params, void* cs_params);
	virtual void* ConfigureADC(const void* driver_params, const int pinA, const int pinB, const int pinC);

	virtual void calibrateOffsets();
};

#endif

