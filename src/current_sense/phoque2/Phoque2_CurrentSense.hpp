#pragma once

#if defined(ARDUINO_PHOQUE2)

#include <current_sense/phoque/Phoque_CurrentSense.hpp>

class Phoque2_CurrentSense : public Phoque_CurrentSense
{
	//Current sense for the Phoque2 board
	//for best performance, center modulation should be off
public:
	Phoque2_CurrentSense(float shunt_resistor, float gain, bool read_bemf=false);
	Phoque2_CurrentSense(float mVpA, bool read_bemf=false);
	virtual ~Phoque2_CurrentSense();

	virtual uint16_t readRaw(const int pin) const override;

private:

	virtual int get_adc1_important_duration() override;
	virtual int get_adc2_important_duration() override;
	virtual int ADC1_Init(ADC_HandleTypeDef* hadc1) override;
	virtual int ADC2_Init(ADC_HandleTypeDef* hadc2) override;

	void clear_currents() final;
};

#endif