#if defined(ARDUINO_PHOQUE)

#include "Phoque_CurrentSense.hpp"
#include <communication/SimpleFOCDebug.h>
#include <current_sense/hardware_specific/stm32/stm32_adc_utils.h>

static constexpr float adc_voltage = 3.3f;
static constexpr int adc_resolution = 4095;

static ADC_HandleTypeDef hadc1;
static ADC_HandleTypeDef hadc2;

static DMA_HandleTypeDef hdma_adc1;
static DMA_HandleTypeDef hdma_adc2;

#define PHOQUE_CS_DEBUG "PHOQUE-CS: "

Phoque_CurrentSense::Phoque_CurrentSense(float _shunt_resistor, float _gain, bool _read_bemf)
	:read_bemf(_read_bemf)
{
	gain_c = gain_b = gain_a = 1.0f /_shunt_resistor / _gain;
}

Phoque_CurrentSense::Phoque_CurrentSense(float _mVpA, bool _read_bemf)
	:read_bemf(_read_bemf)
{
	//Convert millivolts per amp to amps per volt
	gain_c = gain_b = gain_a = 1000.f/_mVpA;
}

Phoque_CurrentSense::~Phoque_CurrentSense()
{
	if (initialized)
	{
		delete[] adc1_buffer;
	}
	
}

void Phoque_CurrentSense::GPIO_Init()
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	__HAL_RCC_ADC12_CLK_ENABLE();
}

void Phoque_CurrentSense::DMA_InitClock() 
{
	/* DMA controller clock enable */
	__HAL_RCC_DMAMUX1_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();

	#ifdef DMA_USE_INTERRUPT
	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/* DMA1_Channel2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
	#endif

	// Enable external clock for ADC
	RCC_PeriphCLKInitTypeDef PeriphClkInit;
	PeriphClkInit.PeriphClockSelection   = RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Adc12ClockSelection    = RCC_ADC12CLKSOURCE_PLL;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

void Phoque_CurrentSense::OPAMP_Init()
{
	//do nothing
}

int Phoque_CurrentSense::get_conversion_duration(int sample_ticks) const
{
	//12.5 ticks of SAR @ 12bits, + half tick of sample
	return sample_ticks + 13;
}

int Phoque_CurrentSense::ADC1_Init(ADC_HandleTypeDef* hadc1)
{
	ADC_MultiModeTypeDef multimode = {0};
	hadc1->Instance = ADC1;
	hadc1->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1->Init.Resolution = ADC_RESOLUTION_12B;
	hadc1->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1->Init.GainCompensation = 0;
	hadc1->Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1->Init.LowPowerAutoWait = DISABLE;
	hadc1->Init.ContinuousConvMode = DISABLE;

	hadc1->Init.DiscontinuousConvMode = DISABLE;
	hadc1->Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
	hadc1->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1->Init.DMAContinuousRequests = ENABLE;
	hadc1->Init.Overrun = ADC_OVR_DATA_PRESERVED;

	if (HAL_ADC_Init(hadc1) != HAL_OK)
	{
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "HAL_ADC_Init failed!");
	}

	/** Configure the ADC multi-mode 
	*/
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(hadc1, &multimode) != HAL_OK)
	{
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "HAL_ADCEx_MultiModeConfigChannel 1 failed!");
	}

	return 0;
}

int Phoque_CurrentSense::ADC2_Init(ADC_HandleTypeDef* hadc2)
{
	hadc2->Instance = ADC2;
	hadc2->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc2->Init.Resolution = ADC_RESOLUTION_12B;
	hadc2->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2->Init.GainCompensation = 0;
	hadc2->Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc2->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc2->Init.LowPowerAutoWait = DISABLE;
	hadc2->Init.ContinuousConvMode = DISABLE;

	hadc2->Init.DiscontinuousConvMode = DISABLE;
	hadc2->Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO2;
	hadc2->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc2->Init.DMAContinuousRequests = ENABLE;
	hadc2->Init.Overrun = ADC_OVR_DATA_PRESERVED;

	if (HAL_ADC_Init(hadc2) != HAL_OK)
	{
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "HAL_ADC_Init failed!");
	}

	return 0;
}

void Phoque_CurrentSense::DMA_Init(ADC_HandleTypeDef *hadc, DMA_HandleTypeDef *hdma_adc, DMA_Channel_TypeDef* channel, uint32_t request)
{
	hdma_adc->Instance = channel;
	hdma_adc->Init.Request = request;
	hdma_adc->Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc->Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc->Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_adc->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma_adc->Init.Mode = DMA_CIRCULAR;
	hdma_adc->Init.Priority = DMA_PRIORITY_LOW;
	HAL_DMA_DeInit(hdma_adc);
	if (HAL_DMA_Init(hdma_adc) != HAL_OK)
	{
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "HAL_DMA_Init failed!");
	}
	__HAL_LINKDMA(hadc, DMA_Handle, *hdma_adc);
}

void* Phoque_CurrentSense::SyncLowSide(void* _driver_params, void* _cs_params)
{
	STM32DriverParams* driver_params = (STM32DriverParams*)_driver_params;
	Stm32CurrentSenseParams* cs_params = (Stm32CurrentSenseParams*)_cs_params;
	TIM_TypeDef *instance = cs_params->timer_handle->Instance;
	 
	// stop all the timers for the driver
	stm32_pause(driver_params);

	//Timer has repetition counter, so this argument is not needed, hence we can let it be garbage
	Stm32AdcInterruptConfig adc_int_config;

	bool tim_interrupt = _initTimerInterruptDownsampling(cs_params, driver_params, adc_int_config);
	if(tim_interrupt) {
	// error in the timer interrupt initialization
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "timer has no repetition counter, ADC interrupt not supported for this Phoque");
		return SIMPLEFOC_CURRENT_SENSE_INIT_FAILED;
	}
	uint32_t adc_ccr = ADC12_COMMON->CCR;
	uint32_t adc_prescaler;
	if (adc_ccr & ADC_CCR_CKMODE_Msk)
	{
		//sync mode
		uint8_t adc_prescalers[] = {1, 1, 2, 4};
		adc_prescaler = adc_prescalers[(adc_ccr & ADC_CCR_CKMODE_Msk) >> ADC_CCR_CKMODE_Pos];
	}
	else
	{
		//async mode
		uint16_t adc_prescalers[] = {1, 2, 4, 6, 8, 10, 12, 16, 32, 64, 128, 256};
		adc_prescaler = adc_prescalers[(adc_ccr & ADC_CCR_PRESC_Msk) >> ADC_CCR_PRESC_Pos];
	}
	
	uint32_t adc_clock_freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_ADC12) / adc_prescaler;
	uint32_t timer_clock_freq = stm32_getTimerClockFreq(cs_params->timer_handle) / (cs_params->timer_handle->Instance->PSC +1);
	uint32_t adc1_center = (uint64_t)get_adc1_important_duration() * timer_clock_freq / adc_clock_freq / 2;
	uint32_t adc2_center = (uint64_t)get_adc2_important_duration() * timer_clock_freq / adc_clock_freq / 2;

	// set the trigger output event
	LL_TIM_OC_InitTypeDef oc_init = {
		.OCMode = LL_TIM_OCMODE_PWM2,
		.OCState = LL_TIM_OCSTATE_DISABLE,
		.OCNState = LL_TIM_OCSTATE_DISABLE,
		.CompareValue = instance->ARR-adc1_center,
		.OCPolarity = LL_TIM_OCPOLARITY_HIGH,
		.OCNPolarity = LL_TIM_OCPOLARITY_HIGH,
		.OCIdleState = LL_TIM_OCIDLESTATE_LOW,
		.OCNIdleState = LL_TIM_OCIDLESTATE_LOW,
	};
	if (adc1_center > 0)
	{
		LL_TIM_OC_Init(instance, LL_TIM_CHANNEL_CH4, &oc_init);
		LL_TIM_SetTriggerOutput(instance, LL_TIM_TRGO_OC4REF);
	}
	else
	{
		LL_TIM_SetTriggerOutput(instance, LL_TIM_TRGO_UPDATE);
	}
	if (adc2_center > 0)
	{
		oc_init.CompareValue = instance->ARR-adc2_center;
		LL_TIM_OC_Init(instance, LL_TIM_CHANNEL_CH6, &oc_init);
		LL_TIM_SetTriggerOutput2(instance, LL_TIM_TRGO2_OC6);
	}
	else
	{
		LL_TIM_SetTriggerOutput2(instance, LL_TIM_TRGO2_UPDATE);
	}

	// restart all the timers of the driver
	stm32_resume(driver_params);

	// return the cs parameters 
	// successfully initialized
	// TODO verify if success in future
	return _cs_params;
}

void* Phoque_CurrentSense::ConfigureADC(const void* driver_params, const int pinA,const int pinB,const int pinC)
{
	_UNUSED(driver_params);

	HAL_Init();
	GPIO_Init();
	DMA_InitClock(); 
	OPAMP_Init();
	hadc1.Init.NbrOfConversion = 0;
	hadc2.Init.NbrOfConversion = 0;
	int adc1_len = ADC1_Init(&hadc1);
	int adc2_len = ADC2_Init(&hadc2);
	//allocate single buffer for faster cache access
	adc1_buffer = new uint16_t[adc1_len+adc2_len];
	adc2_buffer = &adc1_buffer[adc1_len];

	HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);

	DMA_Init(&hadc1, &hdma_adc1, DMA1_Channel1, DMA_REQUEST_ADC1);
	DMA_Init(&hadc2, &hdma_adc2, DMA2_Channel1, DMA_REQUEST_ADC2);

	#ifdef PHOQUE_DEBUG_SAMPLING
	//End of Sample
	LL_ADC_EnableIT_EOSMP(ADC1);
	LL_ADC_EnableIT_EOSMP(ADC2);

	//End of conversion sequence
	LL_ADC_EnableIT_EOS(ADC1);
	LL_ADC_EnableIT_EOS(ADC2);
	#endif

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc1_buffer, adc1_len) != HAL_OK)
	{
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "ADC1 DMA read init failed");
	}
	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc2_buffer, adc2_len) != HAL_OK)
	{
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "ADC2 DMA read init failed");
	}
	
	Stm32CurrentSenseParams* params = new Stm32CurrentSenseParams {
		.pins = { pinA, pinB, pinC },
		.adc_voltage_conv = (adc_voltage) / (adc_resolution),
		.timer_handle = ((STM32DriverParams*)driver_params)->timers_handle[0],
	};

	return params;
}

inline float Phoque_CurrentSense::readMillivolt(const int pin) const
{
	return readRaw(pin) * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv;
}

PhaseCurrent_s Phoque_CurrentSense::getPhaseCurrents(){
	#define compute_current(pin, offset, gain, index) (readMillivolt(pin) - offset)*gain
    PhaseCurrent_s current;
    current.a = compute_current(pinA, offset_ia, gain_a, 0);
    current.b = compute_current(pinB, offset_ib, gain_b, 1);
	current.c = compute_current(pinC, offset_ic, gain_c, 2);
    return current;
}

int Phoque_CurrentSense::init()
{
	if (driver==nullptr) {
		SIMPLEFOC_DEBUG(PHOQUE_CS_DEBUG "Driver not linked!");
		return 0;
	}
	if (initialized)
	{
		return 1;
	}

	// configure ADC variables
	params = ConfigureADC(driver->params,pinA,pinB,pinC);
	// if init failed return fail
	if (params == SIMPLEFOC_CURRENT_SENSE_INIT_FAILED) return 0; 
	// sync the driver
	_delay(10);
	void* r = SyncLowSide(driver->params, params);
	if(r == SIMPLEFOC_CURRENT_SENSE_INIT_FAILED) return 0; 
	// set the center pwm (0 voltage vector)
	if(driver_type==DriverType::BLDC)
	{
		BLDCDriver* driver_bldc = static_cast<BLDCDriver*>(driver);
		const float voltage_target = driver->voltage_limit/2;
		driver_bldc->setPwm(voltage_target, voltage_target, voltage_target);
	}
		
	// calibrate zero offsets
	_delay(10);
	calibrateOffsets();
	// set zero voltage to all phases
	if(driver_type==DriverType::BLDC)
		static_cast<BLDCDriver*>(driver)->setPwm(0,0,0);
	// set the initialized flag
	initialized = (params!=SIMPLEFOC_CURRENT_SENSE_INIT_FAILED);
	// return success
	return 1;
}

void Phoque_CurrentSense::calibrateOffsets(){    
    const int calibration_rounds = 2000;


    // find adc offset = zero current voltage
	uint32_t accA = 0, accB = 0, accC = 0;
    // read the adc voltage 1000 times ( arbitrary number )
    for (int i = 0; i < calibration_rounds; i++) {
		const uint16_t curru(readRaw(pinA)), currv(readRaw(pinB)), currw(readRaw(pinC));
		//check if new data
		if (curru > adc_resolution || currv > adc_resolution || currw > adc_resolution)
		{
			//no new data: this iteration didn't happen
			i--;
			//wait for interrupt (dma irq should do it)
			#ifdef DMA_USE_INTERRUPT
			__WFI();
			#else
			delayMicroseconds(100);
			#endif
			continue;
		}
		//invalidate current data
		clear_currents();
        accA += curru;
        accB += currv;
        accC += currw;
    }
	
    // calculate the mean offsets
    offset_ia = accA * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv / calibration_rounds;
    offset_ib = accB * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv / calibration_rounds;
    offset_ic = accC * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv / calibration_rounds;
	SimpleFOCDebug::printf(PHOQUE_CS_DEBUG "Calibrated centers at %f, %f, %f (gains %f %f %f)\r\n", offset_ia, offset_ib, offset_ic, gain_a, gain_b, gain_c);
}

int Phoque_CurrentSense::driverAlign(float align_voltage, bool modulation_centered)
{
	_UNUSED(align_voltage);
	_UNUSED(modulation_centered);
	#if 1
	return 1;
	#else
	return CurrentSense::driverAlign(align_voltage, modulation_centered);
	#endif
}

#if defined(NTC_B_CONSTANT) && defined(NTC_T0)
#include <array>

#ifndef TEMP_LUT_SIZE
	#define TEMP_LUT_SIZE 7
#endif
#ifndef TEMP_STEP
	#define TEMP_STEP 1 //get every n degrees
#endif
#ifndef TEMP_INDEX_ZERO
	#define TEMP_INDEX_ZERO 20 //start at 20C
#endif

uint16_t temperature_to_adc_reading(float temp_C)
{
	//R0 : resistance at T0
	//R1 : resistance value of the other side of the voltage divider
	//B = ln(R/R0)/(1/T-1/T0) -> R/R0 = exp(B*(1/T-1/T0)) -> R = exp(B*(1/T-1/T0))*R0
	//NTC_DIVIDER_BALANCE = R0/R1
	//V = R/(R+R1)*3.3
	//adc_reading = V/3.3*adc_resolution = R/(R+R1)*adc_resolution = R/(R1(1+R/R1)) * adc_resolution
	float temp_K = temp_C + 273.15f;
	float R_over_R0 = expf(NTC_B_CONSTANT * (1/temp_K-1/NTC_T0));
	#ifdef NTC_DIVIDER_BALANCE
	float R_over_R1 = R_over_R0 / NTC_DIVIDER_BALANCE;
	#else
	float R_over_R1 = R_over_R0;
	#endif
	float adc_reading = R_over_R1/(1+R_over_R1) *adc_resolution;
	return adc_reading;
}

template <class T, size_t N>
std::array<T, N> fill_temperature_lut() {
    std::array<T, N> a;
    for (size_t i = 0; i < N; ++i)
        a[i] = temperature_to_adc_reading((i-1) * TEMP_STEP + TEMP_INDEX_ZERO);
    return a;
}

const auto temperature_lut = fill_temperature_lut<uint16_t, 1<<TEMP_LUT_SIZE>();

int Phoque_CurrentSense::get_temperature(uint16_t adc_value)
{
	uint8_t index = 0;
	for (int8_t i = TEMP_LUT_SIZE-1; i >= 0; i--)
	{
		uint8_t n_index = index | 1<<i;
		if (temperature_lut[n_index] > adc_value)
		{
			index = n_index;
		}
	}
	return index * TEMP_STEP + TEMP_INDEX_ZERO;
}

int Phoque_CurrentSense::read_temperature() const
{
	return get_temperature(readRaw(A_TEMPERATURE));
}
#endif

#ifdef DMA_USE_INTERRUPT
extern "C" {
void DMA1_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_adc1);
}

void DMA2_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_adc2);
}
}
#endif

#endif