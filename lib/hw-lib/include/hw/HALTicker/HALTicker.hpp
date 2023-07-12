#ifndef HALTICKER_H
#define HALTICKER_H

#include "mbed.h"

/**
 * @brief Pure static class that permits to temporize the main cicle. 
 * 
 */
// TODO add a way to calculate the real frequency. 
class HALTicker {
    
public:
    // make impossible to instantiate the class.
    // It admits only static methods
    HALTicker() = delete;
    HALTicker(const HALTicker &) = delete;
    HALTicker &operator=(const HALTicker &) = delete; 

    /// sets the frequency of the loop.
    static void setLoopFreq(float f);

    /// starts the timer, if freq is not equel 0 it sets the frequency
    static bool startTimer(float f = freq);

    /// true if flag_time is != 0
    static bool isReady();

    /// resets the timer
    static void resetFlagTime();

    /// returns the cicle time in mico seconds
    static uint32_t getCicleTime();
    
    /// returns the frequency calculated as 1e6/getCicleTime()
    static float getFreq();

    // set the prescaler value
    static void setPrescalerValue(uint32_t value);

    // returns the prescaler value
    static uint32_t getPrescalerValue(); 

private:
    static volatile int flag_time;
    static TIM_HandleTypeDef mTimUserHandle;
    static uint32_t last_time, act_time;
    static uint32_t prescaler;
    static float freq;

    static void set_time_flag();
};


#endif // HALTICKER_H