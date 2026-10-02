#if defined(ARDUINO_PHOQUE1)


#include "Phoque1_CurrentSense.hpp"
#include "communication/SimpleFOCDebug.h"
#include "current_sense/hardware_specific/stm32/stm32_adc_utils.h"

static OPAMP_HandleTypeDef hopamp1;
static OPAMP_HandleTypeDef hopamp2;
static OPAMP_HandleTypeDef hopamp3;

Phoque1_CurrentSense::Phoque1_CurrentSense(float _shunt_resistor, float _gain, bool _read_bemf)
	:Phoque_CurrentSense(_shunt_resistor, _gain, _read_bemf)
{
	pga_gain = _gain;
	pinA = A_CURRU_H;
	pinB = A_CURRV_H;
	pinC = A_CURRW_H;
}

Phoque1_CurrentSense::Phoque1_CurrentSense(float mVpA, bool _read_bemf)
	:Phoque_CurrentSense(mVpA, _read_bemf)
{
	pinA = A_CURRU_H;
	pinB = A_CURRV_H;
	pinC = A_CURRW_H;
}

Phoque1_CurrentSense::~Phoque1_CurrentSense()
{
}

void Phoque1_CurrentSense::OPAMP_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3 | GPIO_PIN_5|GPIO_PIN_7; //Opamp 1 | Opamp 2
	#if !OPAMP_USE_INTERNAL_CHANNEL
	GPIO_InitStruct.Pin |= GPIO_PIN_2 | GPIO_PIN_6;
	#endif
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2; // Opamp 3
	#if !OPAMP_USE_INTERNAL_CHANNEL
	GPIO_InitStruct.Pin |= GPIO_PIN_1;
	#endif
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	OPAMP_HandleTypeDef *opamp_handles[] = {&hopamp1, &hopamp2, &hopamp3};
	OPAMP_TypeDef *opamp_instances[] = {OPAMP1, OPAMP2, OPAMP3};
	static_assert(sizeof(opamp_handles)/sizeof(opamp_handles[0]) == sizeof(opamp_instances)/sizeof(opamp_instances[0]));

	uint32_t gain_setting = OPAMP_PGA_GAIN_32_OR_MINUS_31;
	switch (pga_gain)
	{
	case 32:
		gain_setting = OPAMP_PGA_GAIN_32_OR_MINUS_31;
		break;
	case 16:
		gain_setting = OPAMP_PGA_GAIN_16_OR_MINUS_15;
		break;
	case 8:
		gain_setting = OPAMP_PGA_GAIN_8_OR_MINUS_7;
		break;
	
	default:
		break;
	}

	for (size_t i = 0; i < sizeof(opamp_handles)/sizeof(opamp_handles[0]); i++)
	{
		auto hopamp = opamp_handles[i];
		hopamp->Instance = opamp_instances[i];
		hopamp->Init = {
			.PowerMode = OPAMP_POWERMODE_HIGHSPEED,
			.Mode = OPAMP_PGA_MODE,
			.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0,
			#if OPAMP_USE_INTERNAL_CHANNEL
			.InternalOutput = ENABLE,
			#else
			.InternalOutput = DISABLE,
			#endif
			.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE,
			.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS,
			.PgaGain = gain_setting,
			.UserTrimming = OPAMP_TRIMMING_FACTORY,
		};
		if (HAL_OPAMP_Init(hopamp) != HAL_OK)
		{
			SimpleFOCDebug::println("HAL_OPAMP_Init failed!");
		}
		HAL_OPAMP_Start(hopamp);
	}
}

#define SAMPLETIME_BULB ADC_SAMPLETIME_640CYCLES_5
#define BULB_CYCLES 640
#define SAMPLETIME_IMPORTANT ADC_SAMPLETIME_6CYCLES_5
#define IMPORTANT_CYCLES 6
#define SAMPLETIME_PERIPHERAL ADC_SAMPLETIME_47CYCLES_5

#if OPAMP_USE_INTERNAL_CHANNEL
	#define ADC1_IMPORTANT_NUM (read_bemf ? 3:1)
	#define ADC2_IMPORTANT_NUM (read_bemf ? 3:2)
#else
	#define ADC1_IMPORTANT_NUM (read_bemf ? 3:2)
	#define ADC2_IMPORTANT_NUM (read_bemf ? 3:1)
#endif

//Start sampling for a long time for the first sample, but sample so that all of the important conversions are centered around update
int Phoque1_CurrentSense::get_adc1_important_duration()
{
	return (ADC1_IMPORTANT_NUM - 1) * get_conversion_duration(IMPORTANT_CYCLES) + BULB_CYCLES*2;
}

int Phoque1_CurrentSense::get_adc2_important_duration()
{
	return (ADC2_IMPORTANT_NUM - 1) * get_conversion_duration(IMPORTANT_CYCLES) + BULB_CYCLES*2;
}

const char *ADC_ConfigFailStart = "HAL_ADC_ConfigChannel failed! ADC";

int Phoque1_CurrentSense::ADC1_Init(ADC_HandleTypeDef* hadc1)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;

	#if OPAMP_USE_INTERNAL_CHANNEL
	hadc1->Init.NbrOfConversion += ADC1_IMPORTANT_NUM + 2;
	#else
	hadc1->Init.NbrOfConversion += ADC1_IMPORTANT_NUM + 1;
	#endif
	Phoque_CurrentSense::ADC1_Init(hadc1);

	
	#if OPAMP_USE_INTERNAL_CHANNEL
	// Configure Internal Channel (Opamp 1 / phase W current)
	sConfig.Channel = _OPAMP_internal_channel_to_ADC(1, ADC1);  // OP1_OUT is ADC1_IN13 for internal channel
	#else
	// Configure regular channel (Opamp 3 / Phase V current)
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_CURRV), ADC1); // OP3_OUT is ADC1_IN12
	#endif
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = SAMPLETIME_BULB;
	if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("1CURRV")
	}

	#if !OPAMP_USE_INTERNAL_CHANNEL
	// Configure regular channel (Opamp 1 / Phase W current)
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_CURRW), ADC1); // OP1_OUT is ADC1_IN3
	sConfig.Rank = ADC_REGULAR_RANK_2;
	sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
	if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("1CURRW")
	}
	#endif

	if(read_bemf)
	{
		/* Configure Regular Channel (PA0 / BEMFU / Phase U)
		*/
		sConfig.Channel = _getADCChannel(analogInputToPinName(A_BEMFU), ADC1);
		#if OPAMP_USE_INTERNAL_CHANNEL
		sConfig.Rank = ADC_REGULAR_RANK_2;
		#else
		sConfig.Rank = ADC_REGULAR_RANK_3;
		#endif
		sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
		if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
		{
			SimpleFOCDebug::print(ADC_ConfigFail);
			SimpleFOCDebug::println("1BEMFU")
		}
	}
	

	//******************************************************************
	// Aux analog readings
	/* Configure Regular Channel (PC1, supply voltage)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_VBUS), ADC1);
	#if OPAMP_USE_INTERNAL_CHANNEL
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_3 : ADC_REGULAR_RANK_2;
	#else
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_4 : ADC_REGULAR_RANK_3;
	#endif
	sConfig.SamplingTime = SAMPLETIME_PERIPHERAL;
	if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("1VBUS")
	}

	#if OPAMP_USE_INTERNAL_CHANNEL
	/** Configure Regular Channel (PC0, Potentiometer)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_POTENTIOMETER), ADC1);
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_4 : ADC_REGULAR_RANK_3;
	sConfig.SamplingTime = SAMPLETIME_PERIPHERAL;
	if (HAL_ADC_ConfigChannel(hadc1, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("1POT")
	}
	#endif
	return hadc1->Init.NbrOfConversion;
}

int Phoque1_CurrentSense::ADC2_Init(ADC_HandleTypeDef* hadc2)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;

	#if OPAMP_USE_INTERNAL_CHANNEL
	hadc2->Init.NbrOfConversion += ADC2_IMPORTANT_NUM + 1;
	#else
	hadc2->Init.NbrOfConversion += ADC2_IMPORTANT_NUM + 2;
	#endif
	Phoque_CurrentSense::ADC2_Init(hadc2);

	#if OPAMP_USE_INTERNAL_CHANNEL
	// Configure Internal Channel (Opamp 2 / phase U current)
	sConfig.Channel = _OPAMP_internal_channel_to_ADC(2, ADC2);  // OP2_OUT is ADC2_IN16 for internal channel
	#else
	// Configure regular channel (Opamp 2 / Phase U current)
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_CURRU), ADC2); // OP2_OUT is ADC2_IN3
	#endif
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = SAMPLETIME_BULB;
	if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("2CURRU")
	}

	#if OPAMP_USE_INTERNAL_CHANNEL
	// Configure Internal Channel (Opamp 3 / phase V current)
	sConfig.Channel = _OPAMP_internal_channel_to_ADC(3, ADC2);     // OP3_OUT is ADC2_IN18 for internal channel
	sConfig.Rank = ADC_REGULAR_RANK_2;
	sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
	if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("2CURRV")
	}
	#endif

	if(read_bemf)
	{
		/* Configure Regular Channel (PC4 / BEMFV / Phase V)
		*/
		sConfig.Channel = _getADCChannel(analogInputToPinName(A_BEMFV), ADC2);
		#if OPAMP_USE_INTERNAL_CHANNEL
		sConfig.Rank = ADC_REGULAR_RANK_3;
		#else
		sConfig.Rank = ADC_REGULAR_RANK_2;
		#endif
		sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
		if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
		{
			SimpleFOCDebug::print(ADC_ConfigFail);
			SimpleFOCDebug::println("2BEMFV")
		}

		/** Configure Regular Channel (PA4 / BEMFW / Phase W)
		*/
		sConfig.Channel = _getADCChannel(analogInputToPinName(A_BEMFW), ADC2);
		#if OPAMP_USE_INTERNAL_CHANNEL
		sConfig.Rank = ADC_REGULAR_RANK_4;
		#else
		sConfig.Rank = ADC_REGULAR_RANK_3;
		#endif
		sConfig.SamplingTime = SAMPLETIME_IMPORTANT;
		if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
		{
			SimpleFOCDebug::print(ADC_ConfigFail);
			SimpleFOCDebug::println("2BEMFW")
		}
	}
	

	/** Configure Regular Channel (PF1 / Mosfet temperature)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_TEMPERATURE), ADC2);
	#if OPAMP_USE_INTERNAL_CHANNEL
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_5 : ADC_REGULAR_RANK_3;
	#else
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_4 : ADC_REGULAR_RANK_2;
	#endif
	sConfig.SamplingTime = SAMPLETIME_PERIPHERAL;
	if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("2TEMP")
	}

	#if !OPAMP_USE_INTERNAL_CHANNEL
	/** Configure Regular Channel (PC0, Potentiometer)
	*/
	sConfig.Channel = _getADCChannel(analogInputToPinName(A_POTENTIOMETER), ADC2);
	sConfig.Rank = read_bemf ? ADC_REGULAR_RANK_5 : ADC_REGULAR_RANK_3;
	sConfig.SamplingTime = SAMPLETIME_PERIPHERAL;
	if (HAL_ADC_ConfigChannel(hadc2, &sConfig) != HAL_OK)
	{
		SimpleFOCDebug::print(ADC_ConfigFail);
		SimpleFOCDebug::println("2POT")
	}
	#endif
	return hadc2->Init.NbrOfConversion;
}

uint16_t Phoque1_CurrentSense::readRaw(const int pin) const
{
    switch (pin)
	{
	case A_CURRU_H:
	case -1:
	case A_CURRU:
		return adc2_buffer[0];
	case A_CURRV_H:
	case -2:
	case A_CURRV:
		#if OPAMP_USE_INTERNAL_CHANNEL
		return adc2_buffer[1];
		#else
		return adc1_buffer[0];
		#endif
	case A_CURRW_H:
	case -3:
	case A_CURRW:
		#if OPAMP_USE_INTERNAL_CHANNEL
		return adc1_buffer[0];
		#else
		return adc1_buffer[1];
		#endif

	case A_BEMFU:
		#if OPAMP_USE_INTERNAL_CHANNEL
		return adc1_buffer[1];
		#else
		return adc1_buffer[2];
		#endif
	case A_BEMFV:
		#if OPAMP_USE_INTERNAL_CHANNEL
		return adc2_buffer[2];
		#else
		return adc2_buffer[1];
		#endif
	case A_BEMFW:
		#if OPAMP_USE_INTERNAL_CHANNEL
		return adc2_buffer[3];
		#else
		return adc2_buffer[2];
		#endif

	case A_VBUS:
		return adc1_buffer[ADC1_IMPORTANT_NUM];
	case A_POTENTIOMETER:
		#if OPAMP_USE_INTERNAL_CHANNEL
		return adc1_buffer[ADC1_IMPORTANT_NUM + 1];
		#else
		return adc2_buffer[ADC2_IMPORTANT_NUM + 1];
		#endif
	case A_TEMPERATURE:
		return adc2_buffer[ADC2_IMPORTANT_NUM];
	default:
		return 0;
	}
}

inline void Phoque1_CurrentSense::clear_currents()
{
	adc1_buffer[0] = UINT16_MAX;
	adc2_buffer[0] = UINT16_MAX;
	#if OPAMP_USE_INTERNAL_CHANNEL
	adc2_buffer[1] = UINT16_MAX;
	#else
	adc1_buffer[1] = UINT16_MAX;
	#endif
}

#endif