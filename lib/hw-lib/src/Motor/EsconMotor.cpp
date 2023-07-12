#include "hw/Motor/EsconMotor.h"

float constrain_float(float val, float min, float max);


EsconMotor::EsconMotor(MotorConfiguration conf, float Kt, float Jm, float maxCurr) : 
    enable(conf.enable), 
    currFeedback(conf.currFeedback),
    analogPhase(conf.analog),
    Jm(Jm),
    Kt(Kt),
    maxCurr(maxCurr),
    offset_bias(conf.offset_bias),
    amp_scale(conf.amp_scale)
{
    //ntd
}

void EsconMotor::setEnable(bool enable)
{
    this->enable = (enable == true) ? 1 : 0;
}

bool EsconMotor::getEnable()
{
    return (enable == 1) ? true : false;
}

void EsconMotor::setCurrent(float curr)
{
    float value = amp_scale * curr + 0.5f + offset_bias; // Centered signal on 50%

#if IC2D_SETUP
    // in this case the analog voltage level is converted to a current between -10 mA -- 10 mA 
    // using a drive circuit for the servovalve, and -10V to 10V for the LinMot. 
    // For example, using the servovalve, at 50% of 3.3V the output current is 0 mA,
    // for 0% is -10 mA, and for 100% is 10 mA
    analogPhase.write(constrain_float(value,0.00,1.00));
#else
    // saturation
    if (value < 0.0f) value = 0.0f;
    else if (value > 1.0f) value = 1.0f;
    analogPhase.write(value);
#endif
}

void EsconMotor::setTorque(float tau_m)
{
    setCurrent( tau_m / Kt );
}

float EsconMotor::getCurrentFeedback() 
{
    return ((currFeedback.read_last_float() - (0.5f + offset_bias) ) / (amp_scale) ) ;
}

float EsconMotor::getTorqueFeedback() 
{    
    return getCurrentFeedback() * Kt;
}


float constrain_float(float val, float min, float max)
{
	if (isnan(val)) return (min + max)/2;

	if (val < min) return min;

	if(val > max) return max;
	
	return val;
}