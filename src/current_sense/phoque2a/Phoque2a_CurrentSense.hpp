#pragma once

#if defined(ARDUINO_PHOQUE2a)

#include <current_sense/phoque/Phoque_CurrentSense.hpp>

class Phoque2a_CurrentSense : public Phoque_CurrentSense
{
	//Current sense for the Phoque2 board
	//for best performance, center modulation should be off
private:
	volatile uint16_t *adc1_buffer = nullptr;
	volatile uint16_t *adc2_buffer = nullptr;
	bool read_bemf;
public:
	Phoque2a_CurrentSense(float shunt_resistor, float gain, bool read_bemf=false);
	Phoque2a_CurrentSense(float mVpA, bool read_bemf=false);
	virtual ~Phoque2a_CurrentSense();

	virtual uint16_t readRaw(const int pin) override;

private:
	virtual int ADC1_Init(ADC_HandleTypeDef* hadc1) override;
	virtual int ADC2_Init(ADC_HandleTypeDef* hadc2) override;
};

#endif