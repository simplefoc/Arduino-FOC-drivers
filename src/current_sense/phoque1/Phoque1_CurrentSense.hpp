#pragma once

#if defined(ARDUINO_PHOQUE1)

#include "current_sense/phoque/Phoque_CurrentSense.hpp"

class Phoque1_CurrentSense : public Phoque_CurrentSense
{
private:
	volatile uint16_t *adc1_buffer = nullptr;
	volatile uint16_t *adc2_buffer = nullptr;
	bool read_bemf;
public:
	Phoque1_CurrentSense(float shunt_resistor, float gain, bool read_bemf=false);
	Phoque1_CurrentSense(float mVpA, bool read_bemf=false);
	~Phoque1_CurrentSense();

	uint16_t readRaw(const int pin);

private:
    void Opamp_Init();
	int ADC1_Init(ADC_HandleTypeDef* hadc1);
	int ADC2_Init(ADC_HandleTypeDef* hadc2);
};

#endif