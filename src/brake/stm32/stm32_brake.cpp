#include "stm32_brake.hpp"

#if defined(HAL_DAC_MODULE_ENABLED) && defined(HAL_COMP_MODULE_ENABLED)
#if !defined(HAL_DAC_MODULE_ONLY)
#warning("HAL_DAC_MODULE_ONLY must be defined for brake to work correctly")
#endif

#include "stm32g4xx_hal_comp.h"
#include "stm32g4xx_hal_dac.h"
#include "Arduino.h" //for pin_function

DAC_TypeDef* stm32_brake::get_dac(uint8_t index)
{
	switch (index)
	{
	#ifdef DAC1
	case 1:
		return DAC1;
	#endif
	#ifdef DAC2
	case 2:
		return DAC2;
	#endif
	#ifdef DAC3
	case 3:
		return DAC3;
	#endif
	#ifdef DAC4
	case 4:
		return DAC4;
	#endif
	default:
		return nullptr;
	}
}

uint32_t stm32_brake::get_dac_channel(uint8_t index)
{
	switch (index)
	{
	#ifdef DAC_CHANNEL_1
	case 1:
		return DAC_CHANNEL_1;
	#endif
	#ifdef DAC_CHANNEL_2
	case 2:
		return DAC_CHANNEL_2;
	#endif
	default:
		return 0;
	}
}

uint32_t stm32_brake::get_comp_minus_dac(uint8_t dac, uint8_t channel)
{
	switch (dac)
	{
	#ifdef DAC1
	case 1:
		switch (channel)
		{
		#ifdef COMP_INPUT_MINUS_DAC1_CH1
		case 1:
			return COMP_INPUT_MINUS_DAC1_CH1;
		#endif
		#ifdef COMP_INPUT_MINUS_DAC1_CH2
		case 2:
			return COMP_INPUT_MINUS_DAC1_CH2;
		#endif
		default:
			return 0;
		}
	#endif
	#ifdef DAC2
	case 2:
		switch (channel)
		{
		#ifdef COMP_INPUT_MINUS_DAC2_CH1
		case 1:
			return COMP_INPUT_MINUS_DAC2_CH1;
		#endif
		#ifdef COMP_INPUT_MINUS_DAC2_CH2
		case 2:
			return COMP_INPUT_MINUS_DAC2_CH2;
		#endif
		default:
			return 0;
		}
	#endif
	#ifdef DAC3
	case 3:
		switch (channel)
		{
		#ifdef COMP_INPUT_MINUS_DAC3_CH1
		case 1:
			return COMP_INPUT_MINUS_DAC3_CH1;
		#endif
		#ifdef COMP_INPUT_MINUS_DAC3_CH2
		case 2:
			return COMP_INPUT_MINUS_DAC3_CH2;
		#endif
		default:
			return 0;
		}
	#endif
	#ifdef DAC4
	case 4:
		switch (channel)
		{
		#ifdef COMP_INPUT_MINUS_DAC4_CH1
		case 1:
			return COMP_INPUT_MINUS_DAC4_CH1;
		#endif
		#ifdef COMP_INPUT_MINUS_DAC4_CH2
		case 2:
			return COMP_INPUT_MINUS_DAC4_CH2;
		#endif
		default:
			return 0;
		}
	#endif
	
	default:
		return 0;
	}
}

COMP_TypeDef* stm32_brake::get_comp_from_plus_io(PinName pin)
{
	switch (pin)
	{
	#ifdef COMP1
	case PA_1:
	case PB_1:
		return COMP1;
	#endif
	#ifdef COMP2
	case PA_7:
	case PA_3:
		return COMP2;
	#endif
	#ifdef COMP3
	case PA_0:
	case PC_1:
		return COMP3;
	#endif
	#ifdef COMP4
	case PB_0:
	case PE_7:
		return COMP4;
	#endif
	#ifdef COMP5
	case PB_13:
	case PD_12:
		return COMP5;
	#endif
	#ifdef COMP6
	case PB_11:
	case PD_11:
		return COMP6;
	#endif
	#ifdef COMP7
	case PB_14:
	case PD_14:
		return COMP7;
	#endif
	
	default:
		return nullptr;
	}
}

uint32_t stm32_brake::get_comp_plus_io(PinName pin)
{
	switch (pin)
	{
	#ifdef COMP1
	case PA_1:
	#endif
	#ifdef COMP2
	case PA_7:
	#endif
	#ifdef COMP3
	case PA_0:
	#endif
	#ifdef COMP4
	case PB_0:
	#endif
	#ifdef COMP5
	case PB_13:
	#endif
	#ifdef COMP6
	case PB_11:
	#endif
	#ifdef COMP7
	case PB_14:
	#endif
		return COMP_INPUT_PLUS_IO1;
	#ifdef COMP1
	case PB_1:
	#endif
	#ifdef COMP2
	case PA_3:
	#endif
	#ifdef COMP3
	case PC_1:
	#endif
	#ifdef COMP4
	case PE_7:
	#endif
	#ifdef COMP5
	case PD_12:
	#endif
	#ifdef COMP6
	case PD_11:
	#endif
	#ifdef COMP7
	case PD_14:
	#endif
		return COMP_INPUT_PLUS_IO2;
	default:
		return 0;
	}
}

DAC_ChannelConfTypeDef stm32_brake::get_dac_config()
{
	DAC_ChannelConfTypeDef dac_channel_config = {
		.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_DISABLE,
		.DAC_DMADoubleDataMode = DISABLE,
		.DAC_SignedFormat = DISABLE,
		.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE,
		.DAC_Trigger = DAC_TRIGGER_NONE,
		.DAC_Trigger2 = DAC_TRIGGER_NONE,
		.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE,
		.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_INTERNAL,
		.DAC_UserTrimming = DAC_TRIMMING_FACTORY,
		.DAC_TrimmingValue = 0
	};
	return dac_channel_config;
}

void stm32_brake::configure_brake(COMP_TypeDef *comparator, bool brake_polarity, uint8_t dac_number, uint8_t _dac_channel, uint32_t hysteresis)
{
	hdac.Instance = get_dac(dac_number);
	
	uint32_t dac_channel = get_dac_channel(_dac_channel);

	hcomp.Instance = comparator;
	hcomp.Init = {
		.InputPlus = get_comp_plus_io(vbus_pin),
		.InputMinus = get_comp_minus_dac(dac_number, _dac_channel),
		.Hysteresis = hysteresis,
		.OutputPol = brake_polarity ? COMP_OUTPUTPOL_NONINVERTED : COMP_OUTPUTPOL_INVERTED,
		.BlankingSrce = COMP_BLANKINGSRC_NONE,
		.TriggerMode = COMP_TRIGGERMODE_NONE
	};
}

stm32_brake::stm32_brake(PinName _vbus_pin, PinName _brake_pin, uint8_t _brake_af_num)
	:vbus_pin(_vbus_pin), brake_pin(_brake_pin), brake_af_num(_brake_af_num)
{

}

void stm32_brake::start_brake(float trigger_voltage_over_vref)
{
	volatile HAL_StatusTypeDef status;
	status = HAL_DAC_Init(&hdac);
	DAC_ChannelConfTypeDef dac_channel_config = get_dac_config();
	status = HAL_DAC_ConfigChannel(&hdac, &dac_channel_config, dac_channel);
	status = set_raw_threshold(trigger_voltage_over_vref);
	status = HAL_DAC_Start(&hdac, dac_channel);
	status = HAL_COMP_Init(&hcomp);
	status = HAL_COMP_Start(&hcomp);
	pin_function(vbus_pin, STM_PIN_DATA(STM_MODE_ANALOG, GPIO_NOPULL, 0));
	pin_function(brake_pin, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, brake_af_num));
}

HAL_StatusTypeDef stm32_brake::set_raw_threshold(float trigger_voltage_over_vref)
{
	return HAL_DAC_SetValue(&hdac, dac_channel, DAC_ALIGN_12B_R, trigger_voltage_over_vref * ((1<<12) -1));
}


#endif