#include "CalibratedSensor.h"


// CalibratedSensor()
// sensor              - instance of original sensor object
CalibratedSensor::CalibratedSensor(Sensor& wrapped) : _wrapped(wrapped) 
{
};


CalibratedSensor::~CalibratedSensor()
{
	delete calibrationLut;
};

// call update of calibrated sensor
void CalibratedSensor::update(){
    _wrapped.update();
    this->Sensor::update();
};

// Retrieve the calibrated sensor angle
void CalibratedSensor::init() {
    // assume wrapped sensor has already been initialized
    this->Sensor::init(); // call superclass init
}

// Retrieve the calibrated sensor angle
float CalibratedSensor::getSensorAngle(){
    // raw encoder position e.g. 0-2PI
    float rawAngle = _wrapped.getMechanicalAngle();

    // index of the bucket that rawAngle is part of. 
    // e.g. rawAngle = 0 --> bucketIndex = 0.
    // e.g. rawAngle = 2PI --> bucketIndex = 128.
    int bucketIndex = floor(rawAngle/(_2PI/n_lut));
    float remainder = rawAngle - ((_2PI/n_lut)*bucketIndex);

    // Extract the lower and upper LUT value in counts
    float y0 = calibrationLut[bucketIndex]; 
    float y1 = calibrationLut[(bucketIndex+1)%n_lut]; 

    // Linear Interpolation Between LUT values y0 and y1 using the remainder
    // If remainder = 0, interpolated offset = y0
    // If remainder = 2PI/n_lut, interpolated offset = y1
    float interpolatedOffset = (((_2PI/n_lut)-remainder)/(_2PI/n_lut))*y0 + (remainder/(_2PI/n_lut))*y1; 

    // add offset to the raw sensor count. Divide multiply by 2PI/CPR to get radians
    float calibratedAngle = rawAngle+interpolatedOffset; 

    // return calibrated angle in radians
    return calibratedAngle;
}

void CalibratedSensor::calibrate(BLDCMotor& motor){
    
    Serial.println("Starting Sensor Calibration.");

    int _NPP = motor.pole_pairs;                                    // number of pole pairs which is user input
    const int n_ticks = 128*_NPP;                                   // number of positions to be sampled per mechanical rotation.  Multiple of NPP for filtering reasons (see later)
    const int n2_ticks = 40;                                        // increments between saved samples (for smoothing motion)
    float deltaElectricalAngle = _2PI*_NPP/(n_ticks*n2_ticks);      // Electrical Angle increments for calibration steps    
    float* error_f  = new float[n_ticks]();                         // pointer to error array rotating forwards
  //  float* raw_f = new float[n_ticks]();                            // pointer to raw forward position
    float* error_b  = new float[n_ticks]();                         // pointer to error array rotating forwards
  //  float* raw_b = new float[n_ticks]();                            // pointer to raw backword position
    float* error = new float[n_ticks]();                            // pointer to error array (average of forward & backward)
    float* error_filt = new float[n_ticks]();                       // pointer to filtered error array (low pass filter)
    const int window = 128;                                         // window size for moving average filter of raw error
    motor.zero_electric_angle = 0;                                  // Set position sensor offset
    
    // find natural direction (this is copy of the init code)
    // move one electrical revolution forward
    for (int i = 0; i <=500; i++ ) {
      float angle = _3PI_2 + _2PI * i / 500.0f;
      motor.setPhaseVoltage(voltage_calibration, 0,  angle);
        _wrapped.update();
      _delay(2);
    }
    // take and angle in the middle
    _wrapped.update();
    float mid_angle = _wrapped.getAngle();
    // move one electrical revolution backwards
    for (int i = 500; i >=0; i-- ) {
      float angle = _3PI_2 + _2PI * i / 500.0f ;
      motor.setPhaseVoltage(voltage_calibration, 0,  angle);
        _wrapped.update();
      _delay(2);
    }
    _wrapped.update();
    float end_angle = _wrapped.getAngle();
    motor.setPhaseVoltage(0, 0, 0);
    _delay(200);
    // determine the direction the sensor moved
    int directionSensor;
    if (mid_angle < end_angle) {
      Serial.println("MOT: sensor_direction==CCW");
      directionSensor = -1;
      motor.sensor_direction = Direction::CCW;

    } else{
      Serial.println("MOT: sensor_direction==CW");
      directionSensor = 1;
      motor.sensor_direction = Direction::CW;

    }

    //Set voltage angle to zero, wait for rotor position to settle
    // keep the motor in position while getting the initial positions
    motor.setPhaseVoltage(voltage_calibration, 0, elec_angle);
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
	Serial.println("Rotating forwards");
	int k = 0;
	for(int i = 0; i<n_ticks; i++)
	{                                                 
		for(int j = 0; j<n2_ticks; j++)
			{   
				elec_angle += deltaElectricalAngle;
				motor.setPhaseVoltage(voltage_calibration, 0, elec_angle);
			}
		
		// delay to settle in position before taking a position sample
		_delay(20);
		_wrapped.update();
		theta_actual = _wrapped.getAngle()-theta_init;
		if (directionSensor == -1)
		{
			theta_actual = -theta_actual;
			error_f[i] = theta_actual - elec_angle/_NPP;

		}
		else
		{
			error_f[i] = elec_angle/_NPP - theta_actual;
		}
		// if overflow happened track it as full rotation
//		raw_f[i] = theta_actual;

		// storing the normalized angle every time the electrical angle 3PI/2 to calculate average zero electrical angle
		if(i==(k*128+96))
			{
				_delay(50);
				avg_elec_angle += _normalizeAngle(directionSensor*_wrapped.getMechanicalAngle()*_NPP);
				k += 1;
			}
	}
	
	_delay(2000);

	/* 
	backwards rotation
	*/
	Serial.println("Rotating backwards");
	for(int i = 0; i<n_ticks; i++)
	{                                                 
		for(int j = 0; j<n2_ticks; j++)
			{   
				elec_angle -= deltaElectricalAngle;
				motor.setPhaseVoltage(voltage_calibration, 0 ,elec_angle);
			}

			// delay to settle in position before taking a position sample
			_delay(20);
			_wrapped.update();
			theta_actual = _wrapped.getAngle()-theta_init;
			if (directionSensor == -1)
			{
			theta_actual = -theta_actual;
			error_b[i] =  theta_actual - elec_angle/_NPP;
			}
			else
			{
			error_b[i] = elec_angle/_NPP - theta_actual;
			}
//			raw_b[i] = theta_actual;
	}

	// get post calibration mechanical angle.
	_wrapped.update();
	theta_absolute_post = _wrapped.getMechanicalAngle();

	// done with the measurement
	motor.setPhaseVoltage(0, 0, 0);

	// raw offset from initial position in absolute radians between 0-2PI
	float raw_offset = (theta_absolute_init+theta_absolute_post)/2;                   

	// calculating the average zero electrica angle from the forward calibration.
	motor.zero_electric_angle  = avg_elec_angle/_NPP;
	Serial.print( "Average Zero Electrical Angle: ");
	Serial.println( motor.zero_electric_angle); 
	
	// Perform filtering to linearize position sensor eccentricity
	// FIR n-sample average, where n = number of samples in one electrical cycle
	// This filter has zero gain at electrical frequency and all integer multiples
	// So cogging effects should be completely filtered out
	float mean = 0;
	for (int i = 0; i<n_ticks; i++){                                            //Average the forward and back directions
		error[i] = 0.5f*(error_f[i] + error_b[n_ticks-i-1]);
		}
	for (int i = 0; i<n_ticks; i++){
		for(int j = 0; j<window; j++){
			int ind = -window/2 + j + i;                                    	// Indexes from -window/2 to + window/2
			if(ind<0){
				ind += n_ticks;}                                                // Moving average wraps around
			else if(ind > n_ticks-1) {
				ind -= n_ticks;}
			error_filt[i] += error[ind]/(float)window;
		}
		mean += error_filt[i]/n_ticks;
	}

	// calculate offset index
	int index_offset = floor(raw_offset/(_2PI/n_lut));

	// Build Look Up Table
	for (int i = 0; i<n_lut; i++){                                          
		int ind =  index_offset + i*directionSensor;
		if(ind > (n_lut-1)){ 
			ind -= n_lut;
		}
		if(ind < 0 ){
			ind += n_lut;
		}
		calibrationLut[ind] = (float) (error_filt[i*_NPP] - mean);
		//Serial.print(ind);
		//Serial.print('\t');
		//Serial.println(calibrationLut[ind],5);
		_delay(1);
	}
   
   // de-allocate memory
    delete error_filt;
    delete error;
  //  delete raw_b;
    delete error_b;
  //  delete raw_f;
    delete error_f;

    Serial.println("Sensor Calibration Done.");

}


