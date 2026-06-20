#pragma once

#if defined(ARDUINO_PHOQUE)

#include "common/base_classes/CurrentSense.h"

class Phoque_CurrentSense : public CurrentSense
{
protected:
	uint16_t *adc1_buffer = nullptr;
	uint16_t *adc2_buffer = nullptr;
	bool read_bemf;
public:
	Phoque_CurrentSense(float shunt_resistor, float gain, bool read_bemf=false);
	Phoque_CurrentSense(float mVpA, bool read_bemf=false);
	virtual ~Phoque_CurrentSense();

	virtual int init() override;
	virtual PhaseCurrent_s getPhaseCurrents() override;
	int driverAlign(float align_voltage, bool modulation_centered = false) final;

	virtual float readMillivolt(const int pin) const;
	virtual uint16_t readRaw(const int pin) const = 0 ;
	#if defined(NTC_B_CONSTANT) && defined(NTC_T0)
	static int get_temperature(uint16_t adc_value);
	int read_temperature() const;
	#endif

protected:
	virtual void GPIO_Init();
	virtual void DMA_InitClock();
	virtual void OPAMP_Init();

	//get duration of conversion in ADC clock cycles
	//sample ticks is the number of clock cycles to sample for, rounded down (for ADC_SAMPLETIME_6CYCLES_5, it's 6)
	int get_conversion_duration(int sample_ticks) const;
	//duration of the sampling for adc1, in ADC clock cycles
	virtual int get_adc1_important_duration() = 0;
	//duration of the sampling for adc2, in ADC clock cycles
	virtual int get_adc2_important_duration() = 0;

	//Configure ADC1, child classes should configure each rank to sample, return number of samples
	virtual int ADC1_Init(ADC_HandleTypeDef* hadc1);
	//Configure ADC2, child classes should configure each rank to sample, return number of samples
	virtual int ADC2_Init(ADC_HandleTypeDef* hadc2);
	virtual void DMA_Init(ADC_HandleTypeDef *hadc, DMA_HandleTypeDef *hdma_adc, DMA_Channel_TypeDef* channel, uint32_t request);
	virtual void* SyncLowSide(void* driver_params, void* cs_params);
	virtual void* ConfigureADC(const void* driver_params, const int pinA, const int pinB, const int pinC);

	virtual void clear_currents() = 0;
	virtual void calibrateOffsets();

};

#endif

