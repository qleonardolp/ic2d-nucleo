#ifndef CONFIG_ESCON_MOTOR_H
#define CONFIG_ESCON_MOTOR_H

namespace motorControl {

    constexpr PinName MOTOR_ENABLE_PIN = PC_10;
    constexpr PinName MOTOR_CURRENT_FEEDBACK_PIN = PA_4;
    constexpr PinName MOTOR_ANALOG_PIN = PA_4;

#ifdef IC2D_SETUP
    constexpr float MAX_CURR = 3.30f; // is it actually the max analog voltage level?
    constexpr float LINMOT_FORCE_MAX = 255.00;
    constexpr float LINMOT_FORCE_MIN = -255.00;
    constexpr float KT = (LINMOT_FORCE_MAX - LINMOT_FORCE_MIN);
    constexpr float JM = 1.0000f;
    constexpr float offset_bias = -0.015; // LinMot: 0.000 | Moog E024: 0.0042 | 0.000
    constexpr float amp_scale = 1.00;
#elif
    constexpr float MAX_CURR = 3.33f;
    constexpr float KT = 1.3112f;
    constexpr float JM = 0.0133f;
    constexpr float offset_bias = 0.0;
    constexpr float amp_scale = 1.0;
#endif

}

#endif // CONFIG_ENV_ESCON_MOTOR_H