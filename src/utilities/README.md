# Utilities

Some classes and functions you may find useful when working with SimpleFOC.

### PreciseAngle

:warning: work in progress! only velocity mode has been tested.

SimpleFOC generally uses floats for calculations. This preserves compatibility with older MCUs which only support 32bit FP, and
can't do double precision, and ensures the maximum compatibilty for the library.

When older MCUs become irrelevant, this may change, but in the meantime there is a problem representing the angles as floats.
On the one hand they have to be precise enough to accurately capture the electric position of the motor's rotor with respect to 
the stator, so meybe even less than 0.1deg. Without this precision you can't do FOC. On the other hand, we need to support many
rotations of the motor shaft - a motor turning at 1000RPM for 1h will be at 60000 rotations or approx. 378,991.118radians.
Expressing these large numbers while also maintaining the required precision is too much for the humble float.

PreciseAngle works in conjunction with PreciseXXXSensor classes [like this one](../encoders/as5048a/PreciseMagneticSensorAS5048A.h).
It solves the float problem by representing the angle as seperate components: angle within rotation and number of rotations, both as
integers. It maintains the complete precision of the sensor over a rotation range of +/- 2^31, i.e. about 2 billion rotations, before
overflowing. 

Even at 50000RPM (which would be a lot for SimpleFOC!) that's enough for about 30 days of continuous rotation. At 1000RPM it's enough 
for 4 years.

