#ifndef CONFIG_ABENCODER_MOTOR_H
#define CONFIG_ABENCODER_MOTOR_H

namespace forecast {
namespace motorEncoder {

#ifdef IC2D_SETUP
    constexpr float CPR = 2000.0f;      // Linear incremental encoder Interpolation Factor (2000)
    constexpr float GEAR_RATIO = 0.50f;  // Using GR as a correction factor
#elif
    constexpr float CPR = 2000000.0f;
    constexpr float GEAR_RATIO = 1.0f;
#endif

}  // namespace motorEncoder
}  // namespace forecast

#endif  // CONFIG_ABENCODER_MOTOR_H