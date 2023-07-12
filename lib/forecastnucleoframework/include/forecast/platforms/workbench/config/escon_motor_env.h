#ifndef CONFIG_ENV_ESCON_MOTOR_H
#define CONFIG_ENV_ESCON_MOTOR_H

namespace envMotor{

    constexpr PinName MOTOR_ENABLE_PIN = PC_11;
    constexpr PinName MOTOR_CURRENT_FEEDBACK_PIN = PA_1;
    constexpr PinName MOTOR_ANALOG_PIN = PA_5;
    constexpr float offset_bias = 0.00;
    constexpr float amp_scale = 1.00;

    // ------- EXO MOTOR ------------------------
    constexpr float MAX_CURR = 2.26f;
    constexpr float KT = 1.5038f;   
    constexpr float JM = 0.0104f;
    constexpr float DM = 0.4f;
}

#endif // CONFIG_CONTROL_ESCON_MOTOR_H