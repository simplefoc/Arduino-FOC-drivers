#if defined(ARDUINO_PHOQUE2)

#include "Phoque2_CurrentSense.hpp"
#include "communication/SimpleFOCDebug.h"
#include "current_sense/hardware_specific/stm32/stm32_adc_utils.h"



Phoque2_CurrentSense::Phoque2_CurrentSense(float _shunt_resistor, float _gain, bool _read_bemf)
	:Phoque_CurrentSense(_shunt_resistor, _gain, _read_bemf)
{
	pinA = A_CURRU;
	pinB = A_CURRV;
	pinC = A_CURRW;
}

Phoque2_CurrentSense::Phoque2_CurrentSense(float mVpA, bool _read_bemf)
	:Phoque_CurrentSense(mVpA, _read_bemf)
{
	pinA = A_CURRU;
	pinB = A_CURRV;
	pinC = A_CURRW;
}

Phoque2_CurrentSense::~Phoque2_CurrentSense()
{
}

#define SAMPLETIME_IMPORTANT ADC_SAMPLETIME_6CYCLES_5
#define SAMPLETIME_PERIPHERAL ADC_SAMPLETIME_47CYCLES_5

int Phoque2_CurrentSense::get_adc1_important_duration()
{
	return (read_bemf ? 2:0) * get_conversion_duration(6) + 6;
}

int Phoque2_CurrentSense::get_adc2_important_duration()
{
	return (read_bemf ? 2:1) * get_conversion_duration(6) + 6;
}


int Phoque2_CurrentSense::ADC1_Init(ADC_HandleTypeDef* hadc1)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;

	hadc1->Init.NbrOfConversion += 3 + read_bemf * 2;

	Phoque_CurrentSense::ADC1_Init(hadc1);

	/** Configure Regular Channel (PA2 / phase U current)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_CURRU), ADC1); //ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
	if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
	}

	if (read_bemf)
	{
		/* Configure Regular Channel (PA1 / BEMFV / Phase V)
		*/
		sConfig.Channel = _getADCChannel(analogInputToPinName(A_BEMFV), ADC1); //ADC_CHANNEL_2;
		sConfig.Rank = ADC_REGULAR_RANK_2;
		sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
		if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
		{
			SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
		}

		/* Configure Regular Channel (PA3 / BEMFW / Phase W)
		*/
		sConfig.Channel = _getADCChannel(analogInputToPinName(A_BEMFW), ADC1); //ADC_CHANNEL_4;
		sConfig.Rank = ADC_REGULAR_RANK_3;
		sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
		if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
		{
			SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
		}
	}

	//******************************************************************
	// Aux analog readings
	/* Configure Regular Channel (PB12, mosfet temperature)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_TEMPERATURE), ADC1); //ADC_CHANNEL_11;
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_4 : ADC_REGULAR_RANK_2;
	sConfig.SamplingTime = SAMPLETIME_PERIPHERAL;
	if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
	}

	/** Configure Regular Channel (PB0 / Bus voltage monitor)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_VBUS), ADC1); //ADC_CHANNEL_15;
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_5 : ADC_REGULAR_RANK_3;
	sConfig.SamplingTime = SAMPLETIME_PERIPHERAL;
	if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
	}
	return hadc1->Init.NbrOfConversion;
}

int Phoque2_CurrentSense::ADC2_Init(ADC_HandleTypeDef* hadc2)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;

	hadc2->Init.NbrOfConversion += 3 + read_bemf;

	Phoque_CurrentSense::ADC2_Init(hadc2);

	/** Configure Regular Channel (PA7 / phase V current)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_CURRV), ADC2); //ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
	if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
	}
	/** Configure Regular Channel (PC4 / phase W current)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_CURRW), ADC2); //ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
	if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
	}

	if(read_bemf)
	{
		/** Configure Regular Channel (PA1 / BEMFU / Phase U)
		*/
		sConfig.Channel = _getADCChannel(analogInputToPinName(A_BEMFU), ADC2); //ADC_CHANNEL_1;
		sConfig.Rank = ADC_REGULAR_RANK_3;
		sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
		if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
		{
			SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
		}
	}

	/** Configure Regular Channel (PB2 / Potentiometer)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_POTENTIOMETER), ADC2); //ADC_CHANNEL_12;
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_4 : ADC_REGULAR_RANK_3;
	sConfig.SamplingTime = SAMPLETIME_PERIPHERAL;
	if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::println("HAL_ADC_ConfigChannel failed!");
	}
	return hadc2->Init.NbrOfConversion;
}

inline uint16_t Phoque2_CurrentSense::readRaw(const int pin) const
{
	switch (pin)
	{
	case A_CURRU:
	case -1:
		return adc1_buffer[0];
	case A_CURRV:
	case -2:
		return adc2_buffer[0];
	case A_CURRW:
	case -3:
		return adc2_buffer[1];

	case A_BEMFU:
		return adc2_buffer[2];
	case A_BEMFV:
		return adc1_buffer[1];
	case A_BEMFW:
		return adc1_buffer[2];

	case A_POTENTIOMETER:
		return adc2_buffer[2+read_bemf];
	case A_TEMPERATURE:
		return adc1_buffer[1+read_bemf*2];
	case A_VBUS:
		return adc1_buffer[2+read_bemf*2];
	default:
		return 0;
	}
}

inline void Phoque2_CurrentSense::clear_currents()
{
	adc1_buffer[0] = UINT16_MAX;
	adc2_buffer[0] = UINT16_MAX;
	adc2_buffer[1] = UINT16_MAX;
}

#endif