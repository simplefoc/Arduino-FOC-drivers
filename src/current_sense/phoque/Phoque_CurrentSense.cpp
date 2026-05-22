#if defined(ARDUINO_PHOQUE)

#include "Phoque_CurrentSense.hpp"
#include <communication/SimpleFOCDebug.h>
#include <current_sense/hardware_specific/stm32/stm32_adc_utils.h>

static constexpr float adc_voltage = 3.3f;
static constexpr float adc_resolution = 4095.f;

static ADC_HandleTypeDef hadc1;
static ADC_HandleTypeDef hadc2;

static DMA_HandleTypeDef hdma_adc1;
static DMA_HandleTypeDef hdma_adc2;

Phoque_CurrentSense::Phoque_CurrentSense(float _shunt_resistor, float _gain, bool _read_bemf)
	:read_bemf(_read_bemf)
{
	gain_c = gain_b = gain_a = 1.0f /_shunt_resistor / _gain;
	skip_align = true;
}

Phoque_CurrentSense::Phoque_CurrentSense(float _mVpA, bool _read_bemf)
	:read_bemf(_read_bemf)
{
	gain_c = gain_b = gain_a = 1000.f/_mVpA;
	skip_align = true;
}

Phoque_CurrentSense::~Phoque_CurrentSense()
{
	if (initialized)
	{
		delete[] adc1_buffer;
		delete[] adc2_buffer;
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

void Phoque_CurrentSense::DMA_Init() 
{
	/* DMA controller clock enable */
	__HAL_RCC_DMAMUX1_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/* DMA1_Channel2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

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

int Phoque_CurrentSense::ADC1_Init(ADC_HandleTypeDef* hadc1)
{
	ADC_MultiModeTypeDef multimode = {0};
	hadc1->Instance = ADC1;
	hadc1->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
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
		SIMPLEFOC_DEBUG("HAL_ADC_Init failed!");
	}

	/** Configure the ADC multi-mode 
	*/
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(hadc1, &multimode) != HAL_OK)
	{
		SIMPLEFOC_DEBUG("HAL_ADCEx_MultiModeConfigChannel 1 failed!");
	}

	return 0;
}

int Phoque_CurrentSense::ADC2_Init(ADC_HandleTypeDef* hadc2)
{
	hadc2->Instance = ADC2;
	hadc2->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
	hadc2->Init.Resolution = ADC_RESOLUTION_12B;
	hadc2->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2->Init.GainCompensation = 0;
	hadc2->Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc2->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc2->Init.LowPowerAutoWait = DISABLE;
	hadc2->Init.ContinuousConvMode = DISABLE;

	hadc2->Init.DiscontinuousConvMode = DISABLE;
	hadc2->Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
	hadc2->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc2->Init.DMAContinuousRequests = ENABLE;
	hadc2->Init.Overrun = ADC_OVR_DATA_PRESERVED;

	if (HAL_ADC_Init(hadc2) != HAL_OK)
	{
		SIMPLEFOC_DEBUG("HAL_ADC_Init failed!");
	}

	return 0;
}

void Phoque_CurrentSense::DMA1_Init(ADC_HandleTypeDef *hadc, DMA_HandleTypeDef *hdma_adc, DMA_Channel_TypeDef* channel, uint32_t request)
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
		SIMPLEFOC_DEBUG("HAL_DMA_Init failed!");
	}
	__HAL_LINKDMA(hadc, DMA_Handle, *hdma_adc);
}

void* Phoque_CurrentSense::SyncLowSide(void* _driver_params, void* _cs_params)
{
	STM32DriverParams* driver_params = (STM32DriverParams*)_driver_params;
	Stm32CurrentSenseParams* cs_params = (Stm32CurrentSenseParams*)_cs_params;
	 
	// stop all the timers for the driver
	stm32_pause(driver_params);

	//Timer has repetition counter, so this argument is not needed, hence we can let it be garbage
	Stm32AdcInterruptConfig adc_int_config;

	bool tim_interrupt = _initTimerInterruptDownsampling(cs_params, driver_params, adc_int_config);
	if(tim_interrupt) {
	// error in the timer interrupt initialization
		SIMPLEFOC_DEBUG("STM32-CS: timer has no repetition counter, ADC interrupt not supported for this Phoque");
		return SIMPLEFOC_CURRENT_SENSE_INIT_FAILED;
	}

	// set the trigger output event
	LL_TIM_SetTriggerOutput(cs_params->timer_handle->Instance, LL_TIM_TRGO_UPDATE);

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
	DMA_Init(); 
	hadc1.Init.NbrOfConversion = 0;
	hadc2.Init.NbrOfConversion = 0;
	int adc1_len = ADC1_Init(&hadc1);
	int adc2_len = ADC2_Init(&hadc2);
	adc1_buffer = new uint16_t[adc1_len];
	adc2_buffer = new uint16_t[adc2_len];

	HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);

	DMA1_Init(&hadc1, &hdma_adc1, DMA1_Channel1, DMA_REQUEST_ADC1);
	DMA1_Init(&hadc2, &hdma_adc2, DMA1_Channel2, DMA_REQUEST_ADC2);

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc1_buffer, adc1_len) != HAL_OK)
	{
		SIMPLEFOC_DEBUG("ADC1 DMA read init failed");
	}
	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc2_buffer, adc2_len) != HAL_OK)
	{
		SIMPLEFOC_DEBUG("ADC2 DMA read init failed");
	}
	
	Stm32CurrentSenseParams* params = new Stm32CurrentSenseParams {
		.pins = { pinA, pinB, pinC },
		.adc_voltage_conv = (adc_voltage) / (adc_resolution),
		.timer_handle = ((STM32DriverParams*)driver_params)->timers_handle[0],
	};

	return params;
}

float Phoque_CurrentSense::readMillivolt(const int pin)
{
	return readRaw(pin) * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv;
}

PhaseCurrent_s Phoque_CurrentSense::getPhaseCurrents(){
    PhaseCurrent_s current;
    current.a = (readMillivolt(pinA) - offset_ia)*gain_a; // amps
    current.b = (readMillivolt(pinB) - offset_ib)*gain_b; // amps
    current.c = (readMillivolt(pinC) - offset_ic)*gain_c; // amps
    return current;
}

int Phoque_CurrentSense::init()
{
	if (driver==nullptr) {
		SIMPLEFOC_DEBUG("CUR: Driver not linked!");
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
		driver_bldc->setPwm(driver->voltage_limit/2, driver->voltage_limit/2, driver->voltage_limit/2);
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
		if (curru >= 0x1000 || currv >= 0x1000 || currw >= 0x1000)
		{
			//no new data: this iteration didn't happen
			i--;
			//wait for interrupt (dma irq should do it)
			__WFI();
			continue;
		}
		//invalidate current data
		adc2_buffer[0] = adc2_buffer[1] = adc1_buffer[0] = 0xffff;
        accA += curru;
        accB += currv;
        accC += currw;
    }
    // calculate the mean offsets
    offset_ia = accA * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv / calibration_rounds;
    offset_ib = accB * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv / calibration_rounds;
    offset_ic = accC * ((Stm32CurrentSenseParams*)params)->adc_voltage_conv / calibration_rounds;
}

int Phoque_CurrentSense::driverAlign(float align_voltage, bool modulation_centered)
{
	_UNUSED(align_voltage);
	_UNUSED(modulation_centered);
	return 0;
}

extern "C" {
void DMA1_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_adc1);
}

void DMA1_Channel2_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_adc2);
}
}

#endif