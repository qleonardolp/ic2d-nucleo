/*
    This library uses TIM2.
    A channel is on PA0
    B channel is on PA1

    Set gearRatio and cpr according to the encoder you are using.
    If you don't have a gear box, set gearRatio to 1 (default value).

    Encoder resolution = gearRatio * countPerRevolution
*/

#include <mbed.h>
#include "DigitalEncoder.h"

#ifndef DIGITAL_ENCODER_AB_H
#define DIGITAL_ENCODER_AB_H

#ifdef IC2D_SETUP
    constexpr uint16_t VELOCITY_PRESCALER = 2;
    constexpr int TIM_RESOLUTION  = 65536;    // 16 bit 
    constexpr float MAX_VELOCITY_PERIOD = 1000000.f;
    constexpr float MAX_OVERFLOW = 1.f;
    constexpr float BASE_VELOCITY = (float)VELOCITY_PRESCALER/90000000.f;
#elif
    constexpr uint16_t VELOCITY_PRESCALER = 2;
    constexpr int TIM_RESOLUTION  = 65536;    // 16 bit 
    constexpr float MAX_VELOCITY_PERIOD = 1000000.f;
    constexpr float MAX_OVERFLOW = 1.f;
    constexpr float BASE_VELOCITY = (float)VELOCITY_PRESCALER/90000000.f;
#endif

class DigitalEncoderAB : public DigitalEncoder {
    public:
        DigitalEncoderAB(float cpr, float gearRatio = 1.0f);

        /**
         * @brief Implementation of the DigitalEncode::getAngle pure virtual
         *        fucntion.
         */
        int getAngle() override;
        /**
         * @brief   Implementation of the DigitalEncode::getMaxAngle pure 
         *          virtual fucntion.
         */
        int getMaxAngle() override;

    #ifdef TARGET_STM32L4
        int getVariation();
    #endif
    
        /**
         * @brief   Implementation of the DigitalEncode::reset pure 
         *          virtual fucntion.
         */
        void reset(int resetOffset = 0) override;

        /**
         * @brief Set the count of interrupt per revolution of the encoder
         */
        void setCountPerRevolution(float cpr);

        /**
         * @brief   Returns the number of interrupt per revolution of the 
         *          encoder
         */
        float getCountPerRevolution();

    #ifdef TARGET_STM32F4 
    // the following timers are not available in other boards
        /**
         * @brief Set the encoder on the hw timer 1
         */
        void setTIM1();

        /**
         * @brief Set the encoder on the hw timer 2
         */
        void setTIM2();

        /**
         * @brief Set the interperiod on the hw timer 3
         */
        void setTIM3();

        /**
         * @brief Set the interperiod on the hw timer 4
         */
        void setTIM4();

        /**
         * @brief Set the encoder on the hw timer 8
         */
        void setTIM8();
    
    #endif // TARGET STM32F4

    #ifdef TARGET_STM32L4
        /**
         * @brief Set the encoder on the low power timer 1
         * 
         * @return true if the inisialization is succesfull
         * @return false otherwise
         */
        bool setLPTIM1(); 

        /**
         * @brief Enable the encoder timer 1
         * 
         * @return true if the inisialization is succesfull
         * @return false otherwise
         */
        bool setVelocity();
    #endif
        
        /**
         * @brief 
         */
        float getVelocityRad(float dt);

    private:
        TIM_TypeDef *VelocityTIM;
        float cpr;
        float period;
        float overflow_correction;

        void EncoderInitialise_TIM1();

    #ifdef TARGET_STM32F4
        int continuous_counter = 0;
        int prec_counter_value = 0;
        TIM_TypeDef *PositionTIM;
        const int offset = TIM_RESOLUTION / 2;

        void EncoderInitialise_TIM2();
        void InterPeriodInitialise_TIM3(); //Used for period measure of TIM1 encoder
        void InterPeriodInitialise_TIM4(); //Used for period measure of TIM8 encoder
        void EncoderInitialise_TIM8();
    #endif

    #ifdef TARGET_STM32L4
        bool direction = false; //false = UP | true = down
        int last_value = 0.0f;
        float angle = 0.0f;

        bool EncoderInitialise_LPTIM1();
    #endif


};
#endif
