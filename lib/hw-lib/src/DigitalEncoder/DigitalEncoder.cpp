#include <hw/DigitalEncoder/DigitalEncoder.h>

#include <cmath>

double DigitalEncoder::getAngleDeg() {
    if(this->angleFormat == AngleFormat::CONTINOUS) {
        return ((double)getAngle()/getMaxAngle())*360.0f;
    } else if(this->angleFormat == AngleFormat::MOD2PI) {
        int angle = getAngle();
        if(angle>=0) {
            while(angle - getMaxAngle() > 0) {
                angle -= getMaxAngle();
            }
        } else {
            while(angle < 0) {
                angle += getMaxAngle();
            }
        }
        return ((double)angle/getMaxAngle())*360.0f;
    }
}

double DigitalEncoder::getAngleRad() {
    if(this->angleFormat == AngleFormat::CONTINOUS) {
        return ((double)getAngle()/getMaxAngle())*2.0f*M_PI;
    } 
    else if(this->angleFormat == AngleFormat::MOD2PI) {
        int angle = getAngle();
        if(angle>=0) {
            while(angle - getMaxAngle() > 0) {
                angle -= getMaxAngle();
            }
        } else {
            while(angle < 0) {
                angle += getMaxAngle();
            }
        }
        return ((double)angle/getMaxAngle())*2.0f*M_PI;
    }
    else if(this->angleFormat == AngleFormat::LINEAR){
        return ((double)getAngle()/getMaxAngle());
    }
}

DigitalEncoder::AngleFormat DigitalEncoder::getAngleFormat() {
    return this->angleFormat;
}   

        
void DigitalEncoder::setAngleFormat(AngleFormat angleFormat) {
    this->angleFormat = angleFormat;
}

void DigitalEncoder::setGearRatio(float gr) {
    gearRatio = gr;
}

float DigitalEncoder::getGearRatio() {
    return gearRatio;
}
