#include <utility/Utility.hpp>
#include <hw/HALTicker/HALTicker.hpp>

using namespace utility;

#ifdef TARGET_NUCLEO_L432KC
#define CLOCK_FREQ      80000000
#define TIM_USR         TIM7
#define TIM_USR_IRQ     TIM7_IRQn
#define TARGET_DEFINED   
#endif

#ifdef TARGET_NUCLEO_F401RE
#define CLOCK_FREQ      84000000 
#define TIM_USR         TIM3
#define TIM_USR_IRQ     TIM3_IRQn
#define TARGET_DEFINED
#endif

#ifdef TARGET_NUCLEO_F446RE
#define CLOCK_FREQ      90000000
#define TIM_USR         TIM7
#define TIM_USR_IRQ     TIM7_IRQn
#define TARGET_DEFINED
#endif

#ifdef TARGET_NUCLEO_F411RE
#define CLOCK_FREQ      100000000
#define TIM_USR         TIM3
#define TIM_USR_IRQ     TIM3_IRQn
#define TARGET_DEFINED
#endif

#ifdef TARGET_NUCLEO_F429ZI
#define CLOCK_FREQ      180000000
#define TIM_USR         TIM2
#define TIM_USR_IRQ     TIM2_IRQn
#define TARGET_DEFINED
#endif

#ifndef TARGET_DEFINED
    #error  "Target board not supported! "\
            "Available targets are: L432KC, F401RE, F446RE, F411RE"
#endif

volatile int HALTicker::flag_time;
TIM_HandleTypeDef HALTicker::mTimUserHandle;
uint32_t HALTicker::act_time;
uint32_t HALTicker::last_time;
float HALTicker::freq = -1.f;

uint32_t HALTicker::prescaler = 1;


void HALTicker::resetFlagTime() {
    flag_time = 0;
}

bool HALTicker::isReady() {
    return not flag_time == 0;
}

uint32_t HALTicker::getCicleTime() {
    return (act_time - last_time);
}

float HALTicker::getFreq() {
    return 1e6 / HALTicker::getCicleTime();
}

void HALTicker::setPrescalerValue(uint32_t value) {
    // the value is a 16 bits and cannot be less than 1
    if (value > 65535 or value < 1) {
        errorState(
            "[HALTicker::setLoopFreq] cannot set a prescaler (%d) < 1!",
            static_cast<int>(value)
        );
    }

    prescaler = value;
}


DigitalOut led(LED1);

void HALTicker::set_time_flag() {
    if (__HAL_TIM_GET_FLAG(&mTimUserHandle, TIM_FLAG_UPDATE) == SET) {
        __HAL_TIM_CLEAR_FLAG(&mTimUserHandle, TIM_FLAG_UPDATE);
        flag_time = 1;
        last_time = act_time;
        act_time = us_ticker_read();
        // static int count = 0;
        // count++;
        // if(count > 3000){
        //     count = 0;
        //     led = !led;
        // }
    }
}

void HALTicker::setLoopFreq(float f)  {
    freq = f;
    uint32_t period = static_cast<uint32_t>(
        ::round((float)CLOCK_FREQ/prescaler/f)
    );

    // TODO I think this is not necessary (noemurr)
    if (f <= 1500 and HALTicker::prescaler <= 1) {
        errorState(
            "[HALTicker::setLoopFreq] Cannot set the required frequency! "
            "please increase the prescaler value (actual=%d)",
            HALTicker::prescaler
        );
    }
    if(period > 65536) {
        // if the prescaler is too low we will fall in this branch.
        errorState(
            "[HALTicker::setLoopFreq] Tim period=%lu > 65536! "
            "try increesing the prescaler value (actual is %lu)",
            period, HALTicker::prescaler
        );
    } 
    if(period < 1) {
        errorState(
            "[HALTicker::setLoopFreq] Tim period=%lu < 1! "
            "try decreesing the prescaler value (actual is %lu)",
            period, HALTicker::prescaler
        );
    }

    mTimUserHandle.Instance             = TIM_USR;
    mTimUserHandle.Init.Prescaler       = HALTicker::prescaler-1;
    mTimUserHandle.Init.CounterMode     = TIM_COUNTERMODE_UP;
    mTimUserHandle.Init.Period          = period-1;
    mTimUserHandle.Init.ClockDivision   = TIM_CLOCKDIVISION_DIV1;
}


bool HALTicker::startTimer(float f) {
    
    if((f <= 0 and freq <= 0)) {
        errorState(
            "[HALTicker::startTimer] Invalid loop frequency: %.3f. "
            "please set the loop frequency with HALTicker::setLoopFreq "
            "if you haven't.", 
            freq
        );

        return false;
    }

    #ifdef TARGET_NUCLEO_F401RE
    __HAL_RCC_TIM3_CLK_ENABLE();    
    #endif

    #ifdef TARGET_NUCLEO_F446RE
    __HAL_RCC_TIM7_CLK_ENABLE();
    #endif

    #ifdef TARGET_NUCLEO_F411RE
    __HAL_RCC_TIM3_CLK_ENABLE();
    #endif

    #ifdef TARGET_NUCLEO_L432KC
    __HAL_RCC_TIM7_CLK_ENABLE();
    #endif

     #ifdef TARGET_NUCLEO_F429ZI
    __HAL_RCC_TIM2_CLK_ENABLE();
    #endif

    if (f != freq) {
        setLoopFreq(f);
    }

    HAL_TIM_Base_Init(&mTimUserHandle);
    HAL_TIM_Base_Start_IT(&mTimUserHandle);

    HAL_NVIC_SetPriority(TIM_USR_IRQ, 2, 0);
    NVIC_SetVector(TIM_USR_IRQ, (uint32_t)set_time_flag);
    NVIC_EnableIRQ(TIM_USR_IRQ);

    resetFlagTime();

    return true;
}