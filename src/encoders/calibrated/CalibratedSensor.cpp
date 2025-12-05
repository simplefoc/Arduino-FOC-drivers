#include "CalibratedSensor.h"

// CalibratedSensor()
// sensor              - instance of original sensor object
// n_lut               - number of samples in the LUT
CalibratedSensor::CalibratedSensor(Sensor &wrapped, int n_lut, uint16_t *lut)
    : _wrapped(wrapped), n_lut(n_lut), allocated(false), calibrationLut(lut) {
		lut_resolution = n_lut / _2PI;
		lut_resolution_inv = 1/ lut_resolution;
	};

CalibratedSensor::~CalibratedSensor() {
	// delete calibrationLut;
	if(allocated) {
		delete []calibrationLut;
	}
};

// call update of calibrated sensor
void CalibratedSensor::update()
{
	_wrapped.update();
	this->Sensor::update();
};

// Retrieve the calibrated sensor angle
void CalibratedSensor::init()
{
	// assume wrapped sensor has already been initialized
	this->Sensor::init(); // call superclass init
}

// Retrieve the calibrated sensor angle
float CalibratedSensor::getSensorAngle()
{
	if(!calibrationLut) {
		return _wrapped.getMechanicalAngle();
	}

    // raw encoder position e.g. 0-2PI
	float raw_angle = _wrapped.getMechanicalAngle();
	// wrap to 0-2PI only if needed (for Encoder sensors that can go beyond 2PI)
	if (raw_angle < 0 || raw_angle >= _2PI) raw_angle = _normalizeAngle(raw_angle);

    // Calculate LUT index
    int lut_index = raw_angle * lut_resolution_inv;

    // Get calibration values from the LUT and decode them
	float lut_entry_lower = decodeOffsetU16(calibrationLut[lut_index]);
	float lut_entry_higher = decodeOffsetU16(calibrationLut[ lut_index >= n_lut-1 ? 0 : (lut_index + 1)]);

    // Linearly interpolate between the two closest LUT entries (one lower and one higher than the raw angle)
	// Calculate the distance between the raw angle and the lower LUT entry
	// Distance is normalized to [0,1]
	float lut_lower_angle = lut_index * lut_resolution;
	float distance_lower = (raw_angle - lut_lower_angle) * lut_resolution_inv;
	// Linearly interpolate between lower and higher LUT entries
	float correction_offset = (1.0f - distance_lower) * lut_entry_lower + distance_lower * lut_entry_higher;

    // Calculate the calibrated angle
    return raw_angle - correction_offset;
}

// Perform filtering to linearize position sensor eccentricity
// FIR n-sample average, where n = number of samples in the window
// This filter has zero gain at electrical frequency and all integer multiples
// So cogging effects should be completely filtered out
void CalibratedSensor::filter_error(float* error, float &error_mean, int n_ticks, int window){
	float window_buffer[window];
	memset(window_buffer, 0, window*sizeof(float));
	float window_sum = 0;
	int buffer_index = 0;
	// fill the inital window buffer
	for (int i = 0; i < window; i++) {
		int ind = n_ticks - window/2 -1 + i;
		window_buffer[i] = error[ind % n_ticks];
		window_sum += window_buffer[i];
	}
	// calculate the moving average
	error_mean = 0;
	for (int i = 0; i < n_ticks; i++)
	{
		// Update buffer
		window_sum -= window_buffer[buffer_index];
		window_buffer[buffer_index] = error[( i + window/2 ) %n_ticks];
		window_sum += window_buffer[buffer_index];
		// update the buffer index
		buffer_index = (buffer_index + 1) % window;

		// Update filtered error
		error[i] = window_sum / (float)window;
		// update the mean value
		error_mean += error[i] / n_ticks;
	}

}

void CalibratedSensor::calibrate(FOCMotor &motor, int settle_time_ms)
{
	// if the LUT is already defined, skip the calibration

	if(calibrationLut == NULL) {
		allocated = true;
		calibrationLut = new uint16_t[n_lut];
	}else{
		SIMPLEFOC_DEBUG("SEN_CAL:Using pre-defined LUT for calibration.");
		return;
	}
	SIMPLEFOC_DEBUG("SEN_CAL:Starting Sensor Calibration.");

	// Calibration variables
	
    // Init inital angles
    float theta_actual = 0.0;
    float avg_elec_angle = 0.0;
	// set the inital electric angle to 0
    float elec_angle = 0.0;

	// Calibration parameters
	// The motor will take a n_pos samples per electrical cycle
	// which amounts to n_ticks (n_pos * motor.pole_pairs) samples per mechanical rotation
	// Additionally, the motor will take n2_ticks steps to reach any of the n_ticks posiitons
	// incrementing the electrical angle by deltaElectricalAngle each time
	int n_pos = 5;
	int _NPP = motor.pole_pairs;								      // number of pole pairs which is user input
	const int n_ticks = n_pos * _NPP;							      // number of positions to be sampled per mechanical rotation.  Multiple of NPP for filtering reasons (see later)
	const int n2_ticks = 5;										      // increments between saved samples (for smoothing motion)
	float deltaElectricalAngle = _2PI * _NPP / (n_ticks * n2_ticks);  // Electrical Angle increments for calibration steps
	float error[n_ticks];	         						  		  // pointer to error array (average of forward & backward)
	memset(error, 0, n_ticks*sizeof(float));
	// The fileter window size is set to n_pos - one electrical cycle 
	// important for cogging filtering !!!
	const int window = n_pos; // window size for moving average filter of raw error
	

	// find the first guess of the motor.zero_electric_angle 
	// and the sensor direction
	// updates motor.zero_electric_angle
	// updates motor.sensor_direction
	// temporarily unlink the sensor and current sense
	CurrentSense *current_sense = motor.current_sense;
	motor.current_sense = nullptr;
	motor.linkSensor(&this->_wrapped);
	if(!motor.initFOC()){
		SIMPLEFOC_DEBUG("SEN_CAL: Failed to align the sensor.");
		return;
	}
	// link back the sensor and current sense
	motor.linkSensor(this);
	motor.linkCurrentSense(current_sense);

	// Set voltage angle to zero, wait for rotor position to settle
	// keep the motor in position while getting the initial positions
	motor.setPhaseVoltage(1, 0, elec_angle);
	_delay(1000);
	_wrapped.update();
	float theta_init = _wrapped.getAngle();
	float theta_absolute_init = _wrapped.getMechanicalAngle();

	/*
	Start Calibration
	Loop over  electrical angles from 0 to NPP*2PI, once forward, once backward
	store actual position and error as compared to electrical angle
	*/

	/*
	forwards rotation
	*/
	SIMPLEFOC_DEBUG(motor.sensor_direction == Direction::CCW ? "Rotating: CCW" : "Rotating: CW" );
	float zero_angle_prev = 0.0;
	for (int i = 0; i < n_ticks; i++)
	{
		for (int j = 0; j < n2_ticks; j++) // move to the next location
		{
			_wrapped.update();
			elec_angle += deltaElectricalAngle;
			motor.setPhaseVoltage(voltage_calibration, 0, elec_angle);
		}

		// delay to settle in position before taking a position sample
		_delay(settle_time_ms);
		_wrapped.update();
		// calculate the error
		theta_actual = (int)motor.sensor_direction * (_wrapped.getAngle() - theta_init);
        error[i] = 0.5 * (theta_actual - elec_angle / _NPP);

		// calculate the current electrical zero angle
		float zero_angle = ((int)motor.sensor_direction * _wrapped.getMechanicalAngle() * _NPP ) - (elec_angle + _PI_2);
		zero_angle = _normalizeAngle(zero_angle);
		// remove the 2PI jumps
		if(zero_angle - zero_angle_prev > _PI){
			zero_angle = zero_angle - _2PI;
		}else if(zero_angle - zero_angle_prev < -_PI){
			zero_angle = zero_angle + _2PI;
		}
		zero_angle_prev = zero_angle;
		avg_elec_angle += zero_angle/n_ticks;

#ifdef SIMPLEFOC_CALIBRATEDSENSOR_DEBUG
		SIMPLEFOC_DEBUG(">zero:",zero_angle);
		SIMPLEFOC_DEBUG(">zero_average:", (float)(avg_elec_angle/2.0));
#endif
	}
	_delay(2000);

	/*
	backwards rotation
	*/
	SIMPLEFOC_DEBUG(motor.sensor_direction == Direction::CCW ? "Rotating: CW" : "Rotating: CCW" );
	for (int i = n_ticks - 1; i >= 0; i--)
	{
		for (int j = 0; j < n2_ticks; j++) // move to the next location
		{
			_wrapped.update();
			elec_angle -= deltaElectricalAngle;
			motor.setPhaseVoltage(voltage_calibration, 0, elec_angle);
		}

		// delay to settle in position before taking a position sample
		_delay(settle_time_ms);
		_wrapped.update();
		// calculate the error
		theta_actual = (int)motor.sensor_direction * (_wrapped.getAngle() - theta_init);
        error[i] += 0.5 * (theta_actual - elec_angle / _NPP);
		// calculate the current electrical zero angle
		float zero_angle = ((int)motor.sensor_direction * _wrapped.getMechanicalAngle() * _NPP ) - (elec_angle + _PI_2);
		zero_angle = _normalizeAngle(zero_angle);
		// remove the 2PI jumps
		if(zero_angle - zero_angle_prev > _PI){
			zero_angle = zero_angle - _2PI;
		}else if(zero_angle - zero_angle_prev < -_PI){
			zero_angle = zero_angle + _2PI;
		}
		zero_angle_prev = zero_angle;
		avg_elec_angle += zero_angle/n_ticks;
#ifdef SIMPLEFOC_CALIBRATEDSENSOR_DEBUG
		SIMPLEFOC_DEBUG(">zero:", zero_angle);
		SIMPLEFOC_DEBUG(">zero_average:",  (float)(avg_elec_angle/2.0));
#endif
	}

	// get post calibration mechanical angle.
	_wrapped.update();
	float theta_absolute_post = _wrapped.getMechanicalAngle();

	// done with the measurement
	motor.setPhaseVoltage(0, 0, 0);

	// raw offset from initial position in absolute radians between 0-2PI
	float raw_offset = (theta_absolute_init + theta_absolute_post) / 2;

	// calculating the average zero electrical angle from the forward calibration.
	motor.zero_electric_angle = _normalizeAngle(avg_elec_angle / (2.0));
	SIMPLEFOC_DEBUG("SEN_CAL:Average Zero Electrical Angle: ", motor.zero_electric_angle);
	_delay(1000);

	// Perform filtering to linearize position sensor eccentricity
	// FIR n-sample average, where n = number of samples in one electrical cycle
	// This filter has zero gain at electrical frequency and all integer multiples
	// So cogging effects should be completely filtered out
	float error_mean = 0.0;
	this->filter_error(error, error_mean, n_ticks, window);

	_delay(1000);
	// calculate offset index
	int index_offset = floor((float)n_lut * raw_offset / _2PI);
	float dn = n_ticks / (float)n_lut;

	SIMPLEFOC_DEBUG("SEN_CAL: Constructing LUT.");
	_delay(1000);
	// Build Look Up Table
	for (int i = 0; i < n_lut; i++)
	{
		int ind = index_offset + i*motor.sensor_direction;
		if (ind > (n_lut - 1)) ind -= n_lut;
		if (ind < 0) ind += n_lut;
		float offset_value = (float)(error[(int)(i * dn)] - error_mean); 
		// negate the error if the sensor is in the opposite direction
		offset_value =  (int)motor.sensor_direction * offset_value;
		// encode to uint16_t
		calibrationLut[ind] = encodeOffsetU16(offset_value);
	}
	_delay(1000);

	SIMPLEFOC_DEBUG("SEN_CAL: Sensor Calibration Done.");

	// pre-compute inverse LUT resolution for faster lookups
	lut_resolution = _2PI / n_lut;
	lut_resolution_inv = 1.0 / lut_resolution;
}

// print the LUT for debugging
void CalibratedSensor::printLUT(FOCMotor &motor, Print &printer)
{
	// Display the LUT
	printer.print("uint16_t calibrationLut[");
	printer.print(n_lut);
	printer.println("] = {");
	_delay(100); 
	for (int i=0;i < n_lut; i++){
		printer.print(calibrationLut[i]);
		if(i < n_lut - 1) printer.print(", ");
		_delay(1);
	}
	printer.println("");
	printer.println("};");
	_delay(1000);

	// Display the zero electrical angle
	printer.print("float zero_electric_angle = ");
	printer.print(motor.zero_electric_angle);
	printer.println(";");

	// Display the sensor direction
	printer.print("Direction sensor_direction = ");
	printer.println(motor.sensor_direction == Direction::CCW ? "Direction::CCW;" : "Direction::CW;");
	_delay(1000);
}