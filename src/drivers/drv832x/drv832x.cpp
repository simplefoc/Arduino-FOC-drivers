

#include "./drv832x.h"

#define BIT_READ_MASK 0x07FF


void DRV832xDriver3PWM::init(SPIClass* _spi) {
	DRV832xDriver::init(_spi);
	setRegistersLocked(false);
	delayMicroseconds(1);
	DRV832xDriver::setPWMMode(DRV832x_PWMMode::PWM3_Mode);
	BLDCDriver3PWM::init();
};


void DRV832xDriver6PWM::init(SPIClass* _spi) {
	DRV832xDriver::init(_spi);
	setRegistersLocked(false);
	delayMicroseconds(1);
	DRV832xDriver::setPWMMode(DRV832x_PWMMode::PWM6_Mode); // default mode is 6-PWM
	BLDCDriver6PWM::init();
};

void DRV832xDriver::init(SPIClass* _spi) {
	// TODO make SPI speed configurable
	spi = _spi;
	settings = SPISettings(1000000, MSBFIRST, SPI_MODE1);

	//setup pins
	pinMode(cs, OUTPUT);
	digitalWrite(cs, HIGH); // switch off

	//SPI has an internal SPI-device counter, it is possible to call "begin()" from different devices
	spi->begin();

	if (_isset(nFault)) {
		pinMode(nFault, INPUT);
		// TODO add interrupt handler on the nFault pin if configured
		// add configuration for how to handle faults... idea: interrupt handler calls a callback, depending on the type of fault
		// consider what would be a useful configuration in practice? What do we want to do on a fault, e.g. over-temperature for example?

		//attachInterrupt(digitalPinToInterrupt(nFault), handleInterrupt, PinStatus::FALLING);
	}
	/*for (size_t i = 0; i < 7; i++)
	{
		auto read = readSPI(i);
		Serial.printf("DRV8323 Register %d = %#x\n", i, read);
	}*/
};




uint16_t DRV832xDriver::readSPI(uint8_t addr) {
	digitalWrite(cs, 0);
	spi->beginTransaction(settings);
	addr &= 0xf;
	uint16_t data = (addr<<11)|0x8000;
	uint16_t result = spi->transfer16(data);
	spi->endTransaction();
	digitalWrite(cs, 1);
	//Serial.printf("SPI read result: @%#x -> %#x\n", data, result);
	return result;
}


uint16_t DRV832xDriver::writeSPI(uint8_t addr, uint16_t value) {
	digitalWrite(cs, 0);
	spi->beginTransaction(settings);
	addr &= 0xf;
	value &= BIT_READ_MASK;
	uint16_t data = (addr<<11)|value;
	uint16_t result = spi->transfer16(data);
	spi->endTransaction();
	digitalWrite(cs, 1);
	//Serial.printf("SPI write result: @%#x -> %#x\n", data, result);
	return result;
}

#define def_getset(name, type, address, register_type, register_name) \
type DRV832xDriver::get##name() {\
	register_type data;\
	data.reg = readSPI(address) & BIT_READ_MASK;\
	return static_cast<type>(data.register_name);\
}\
\
void DRV832xDriver::set##name(type value) {\
	register_type data;\
	data.reg = readSPI(address) & BIT_READ_MASK;\
	data.register_name = value;\
	delayMicroseconds(1);\
	writeSPI(address, data.reg);\
}

#define def_isset(name, address, register_type, register_name) \
bool DRV832xDriver::is##name() {\
	register_type data;\
	data.reg = readSPI(address) & BIT_READ_MASK;\
	return static_cast<bool>(data.register_name);\
}\
\
void DRV832xDriver::set##name(bool value) {\
	register_type data;\
	data.reg = readSPI(address) & BIT_READ_MASK;\
	data.register_name = value;\
	delayMicroseconds(1);\
	writeSPI(address, data.reg);\
}

DRV832xStatus DRV832xDriver::getStatus() {
	Fault data0;
	VGS data1;
	uint16_t result = readSPI(Fault_Status_ADDR);
	data0.reg = (result & BIT_READ_MASK);
	delayMicroseconds(1); // delay at least 400ns between operations
	result = readSPI(VGS_Status_ADDR);
	data1.reg = (result & BIT_READ_MASK);
	return DRV832xStatus(data0, data1);
}


void DRV832xDriver::clearFault() {
	uint16_t result = readSPI(Driver_Control_ADDR);
	Driver_Control data;
	data.reg = (result & BIT_READ_MASK);
	data.CLR_FLT |= 1;
	delayMicroseconds(1); // delay at least 400ns between operations
	result = writeSPI(Driver_Control_ADDR, data.reg);
}


bool DRV832xDriver::isRegistersLocked(){
	uint16_t result = readSPI(Gate_Drive_HS_ADDR);
	Gate_Drive_HS data;
	data.reg = (result & BIT_READ_MASK);
	return data.LOCK==LOCK_LOCK;
}
void DRV832xDriver::setRegistersLocked(bool lock){
	uint16_t result = readSPI(Gate_Drive_HS_ADDR);
	Gate_Drive_HS data;
	data.reg = (result & BIT_READ_MASK);
	data.LOCK = lock?LOCK_LOCK:LOCK_UNLOCK;
	delayMicroseconds(1); // delay at least 400ns between operations
	result = writeSPI(Gate_Drive_HS_ADDR, data.reg);
}

def_isset(Brake, Driver_Control_ADDR, Driver_Control, BRAKE)
def_isset(Coast, Driver_Control_ADDR, Driver_Control, COAST)
def_isset(OnePWMInvertDirection, Driver_Control_ADDR, Driver_Control, _1PWM_DIR)
def_isset(OnePWMAsynchronousRectification, Driver_Control_ADDR, Driver_Control, _1PWM_COM)
def_getset(PWMMode, DRV832x_PWMMode, Driver_Control_ADDR, Driver_Control, PWM_MODE)
def_isset(OvertemperatureReporting, Driver_Control_ADDR, Driver_Control, OTW_REP)
def_isset(DisableGateDriveFault, Driver_Control_ADDR, Driver_Control, DIS_GDF)
def_isset(DisableChargePumpUndervoltage, Driver_Control_ADDR, Driver_Control, DIS_CPUV)

def_getset(VDSLevel, DRV832x_VDS_LVL, OCP_Control_ADDR, OCP_Control, VDS_LVL);
def_getset(OCPMode, DRV832x_OCPMode, OCP_Control_ADDR, OCP_Control, OCP_MODE)
def_getset(OCPDeglitchTime, DRV832x_OCPDeglitch, OCP_Control_ADDR, OCP_Control, OCP_DEG)
def_getset(DeadTime, DRV832x_DEADTIME, OCP_Control_ADDR, OCP_Control, DEAD_TIME);

def_getset(HighSideChargeCurrent, DRV832x_IDRIVEP, Gate_Drive_HS_ADDR, Gate_Drive_HS, IDRIVEP_HS);
def_getset(HighSideDischargeCurrent, DRV832x_IDRIVEN, Gate_Drive_HS_ADDR, Gate_Drive_HS, IDRIVEN_HS);
def_getset(LowSideChargeCurrent, DRV832x_IDRIVEP, Gate_Drive_LS_ADDR, Gate_Drive_LS, IDRIVEP_LS);
def_getset(LowSideDischargeCurrent, DRV832x_IDRIVEN, Gate_Drive_LS_ADDR, Gate_Drive_LS, IDRIVEN_LS);

def_getset(DriveTime, DRV832x_TDRIVE, Gate_Drive_LS_ADDR, Gate_Drive_LS, TDRIVE);
def_isset(CycleByCycle, Gate_Drive_LS_ADDR, Gate_Drive_LS, CBC);
def_isset(RetryTime, OCP_Control_ADDR, OCP_Control, TRETRY);

def_getset(CurrentSenseOvercurrentSensitivity, DRV832x_CS_VSEN_LVL, CSA_Control_ADDR, CSA_Control, SEN_LVL);
def_isset(CurrentSenseCalibrateA, CSA_Control_ADDR, CSA_Control, CSA_CAL_A);
def_isset(CurrentSenseCalibrateB, CSA_Control_ADDR, CSA_Control, CSA_CAL_B);
def_isset(CurrentSenseCalibrateC, CSA_Control_ADDR, CSA_Control, CSA_CAL_C);
def_isset(CurrentSenseOvercurrentDisable, CSA_Control_ADDR, CSA_Control, DIS_SEN);
def_getset(CurrentSenseGain, DRV832x_CSAGain, CSA_Control_ADDR, CSA_Control, CSA_GAIN)
def_isset(CurrentSenseOvercurrentResistor, CSA_Control_ADDR, CSA_Control, LS_REF);
def_isset(CurrentSenseBidirectionnal, CSA_Control_ADDR, CSA_Control, VREF_DIV);
def_isset(CurrentSenseFET, CSA_Control_ADDR, CSA_Control, CSA_FET);


