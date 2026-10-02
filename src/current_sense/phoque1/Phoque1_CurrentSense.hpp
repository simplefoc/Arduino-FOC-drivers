#pragma once

#if defined(ARDUINO_PHOQUE1)

#include "current_sense/phoque/Phoque_CurrentSense.hpp"
#include <stm32g4xx.h>

class Phoque1_CurrentSense : public Phoque_CurrentSense
{
private:
	uint8_t pga_gain = 32;

public:
	Phoque1_CurrentSense(float shunt_resistor, float gain, bool read_bemf=false);
	Phoque1_CurrentSense(float mVpA, bool read_bemf=false);
	~Phoque1_CurrentSense();

	virtual uint16_t readRaw(const int pin) const override;

	static constexpr float compute_equivalent_shunt(float rshunt = 3e-3f, float rline = 1.5e3f, float rup = 100e3f, float rdown = INFINITY) //On bg431-esc1, this is 3m, 1.5k, 22k, 2.2k
	{
		float req1 = isinff(rdown) ? rup : rup*rdown/(rup+rdown);
		return rshunt*req1/(rshunt+rline+req1);
	}
	static constexpr uint32_t OPAMP_get_internal_channel_to_ADC(uint8_t opamp_idx, ADC_TypeDef *ADC)
	{
		#if defined(STM32G4xx)
		switch (opamp_idx)
		{
		case 1:
			return ADC == ADC1 ? ADC_CHANNEL_13 : 0; //adc1
		case 2:
			return ADC == ADC2 ? ADC_CHANNEL_16 : 0; //adc2
		#ifdef OPAMP3
		case 3:
			switch ((intptr_t)ADC)
			{
			case ADC2_BASE:
				return ADC_CHANNEL_18;
			#ifdef ADC3
			case ADC3_BASE:
				return ADC_CHANNEL_13;
			#endif
			default:
				return 0;
			}
		#endif
		#ifdef OPAMP4
			return ADC == ADC5 ? ADC_CHANNEL_5 : 0; //adc5
		#endif
		#ifdef OPAMP5
			return ADC == ADC5 ? ADC_CHANNEL_3 : 0; //adc5
		#endif
		#ifdef OPAMP6
			switch ((intptr_t)ADC)
			{
			#ifdef ADC3
			case ADC3_BASE:
				return ADC_CHANNEL_17; //For stm32g3x4
			#endif
			#ifdef ADC4
			case ADC4_BASE:
				return ADC_CHANNEL_17; //For stm32g4x3
			#endif
			default:
				return 0;
			}
		#endif
		default:
			return 0;
		}
		#endif
		return 0;
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