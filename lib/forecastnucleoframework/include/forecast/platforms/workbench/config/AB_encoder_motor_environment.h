#ifndef CONFIG_ABENCODER_MOTOR_ENV_H
#define CONFIG_ABENCODER_MOTOR_ENV_H

namespace forecast {
namespace envMotorEncoder {

constexpr float CPR = 4096.0f;
constexpr float GEAR_RATIO = 371250.0f/3610.0f;

}  // namespace envMotorEncoder
}  // namespace forecast

#endif  // CONFIG_ABENCODER_MOTOR_ENV_H