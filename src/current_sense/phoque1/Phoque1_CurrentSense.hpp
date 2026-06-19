#pragma once

#if defined(ARDUINO_PHOQUE1)

#include "current_sense/phoque/Phoque_CurrentSense.hpp"

class Phoque1_CurrentSense : public Phoque_CurrentSense
{
public:
	Phoque1_CurrentSense(float shunt_resistor, float gain, bool read_bemf=false);
	Phoque1_CurrentSense(float mVpA, bool read_bemf=false);
	~Phoque1_CurrentSense();

	virtual uint16_t readRaw(const int pin) const override;

	static constexpr float compute_equivalent_shunt(float rshunt = 3e-3f, float rline = 1.5e3f, float rup = 2.2e3f, float rdown = 22e3f)
	{
		float req1 = rup*rdown/(rup+rdown);
		return rshunt*req1/(rshunt+rline+req1);
	}

protected:
    virtual void OPAMP_Init() override;

	//duration of the sampling for adc1, in ADC clock cycles
	virtual int get_adc1_important_duration() override;
	//duration of the sampling for adc2, in ADC clock cycles
	virtual int get_adc2_important_duration() override;
	virtual int ADC1_Init(ADC_HandleTypeDef* hadc1) override;
	virtual int ADC2_Init(ADC_HandleTypeDef* hadc2) override;

	void clear_currents() final;
};

#endif