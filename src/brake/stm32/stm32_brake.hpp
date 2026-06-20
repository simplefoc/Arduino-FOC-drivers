#pragma once

#ifdef STM32G4xx
#include "stm32g4xx_hal.h"
#if defined(HAL_DAC_MODULE_ENABLED) && defined(HAL_COMP_MODULE_ENABLED)

#include "PinNames.h"
#include "stm32g4xx_hal_comp.h"
#include "stm32g4xx_hal_dac.h"

class stm32_brake
{
protected:
	DAC_HandleTypeDef hdac;
	COMP_HandleTypeDef hcomp;
	//stm32 hardware dac channel, not 1-2
	uint32_t dac_channel;
	PinName vbus_pin;
	PinName brake_pin;
	uint8_t brake_af_num;
public:
	static DAC_TypeDef* get_dac(uint8_t index);
	static uint32_t get_dac_channel(uint8_t index);
	static uint32_t get_comp_minus_dac(uint8_t dac, uint8_t channel);
	static COMP_TypeDef* get_comp_from_plus_io(PinName pin);
	static uint32_t get_comp_plus_io(PinName pin);
protected:

	virtual DAC_ChannelConfTypeDef get_dac_config();
	/**
	 * @brief Configure DAC and comparator for brake function
	 * 
	 * @param comparator Instance of the comparator to use
	 * @param brake_polarity high if brake is applied when pin is high
	 * @param dac_number Index of the DAC to use (for DAC1, this is 1)
	 * @param dac_channel Index of the channel to use (for channel 1, this is 1)
	 * @param hysteresis Hysteresis of the brake, can be value of @ref COMP_Hysteresis
	 */
	void configure_brake(COMP_TypeDef *comparator, bool brake_polarity, uint8_t dac_number, uint8_t _dac_channel, uint32_t hysteresis);

public:
	/**
	 * @brief Start comparator and DAC
	 * 
	 * @param trigger_voltage_over_vref Normalised voltage at which to trigger the comparator: if vbus_pin sees Vbus/10 and vref is 3.3, and you want to trigger at 20V, set to 20/10/3.3
	 */
	stm32_brake(PinName vbus_pin, PinName brake_pin, uint8_t brake_af_num);
	
	void start_brake(float trigger_voltage_over_vref);
	HAL_StatusTypeDef set_raw_threshold(float trigger_voltage_over_vref);
};
#endif
#endif