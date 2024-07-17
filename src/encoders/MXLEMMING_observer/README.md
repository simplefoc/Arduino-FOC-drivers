# MXLEMMING Observer Sensor

The MXLEMMING Obserser has been ported from the MESC Firmware, it's also the default Flux observer in Vesc firmware.
The [MESC book](https://davidmolony.github.io/MESC_Firmware/operation/CONTROL.html#the-sensorless-observer) explains the math behind this flux observer.

It's a simple solution for sensorless motor control only using phase currents and motor parameters, if tracking the position at low speed and when not driving the motor is not relevant.

### Motor Parameters:
The MXLEMMING Observer needs the following motor parameters to be set:
- phase resistance
- KV rating
- phase inductance
- pole pairs

It will not track the position if any of those parameters are missing.

The KV rating and pole pairs parameters are used to derive the motor flux linkage which is key for the flux observer to run well.
```
BLDCMotor motor = BLDCMotor(15, 0.1664, 17.0, 0.00036858); // Hoverboard Motor
MXLEMMINGObserverSensor sensor = MXLEMMINGObserverSensor(motor);
```
flux_linkage parameter can be adjusted from the code.

### Current Sense
The current sense is required as this flux observer only relies on phase currents.

### Sensor Alignment
The flux observer sensor doesn't need sensor alignment.
```
motor.sensor_direction= Direction::CW;
motor.zero_electric_angle = 0;
```

### Bemf Threshold
The sensor also has a bemf_threshold parameter (0 by default) that prevents the flux observer from tracking if the estimated bemf is not high enough.
This can help when starting the motor as the flux observer is not good at tracking the position at low speed.

### To do:
The Clarke transform is running both in the loopFOC and in the sensor update now, it can be remove from the sensor when the Alpha and Beta currents will be persisted as a BLDCMotor member.

