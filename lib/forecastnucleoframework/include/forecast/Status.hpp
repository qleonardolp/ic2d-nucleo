#ifndef HW_CMDS_HPP
#define HW_CMDS_HPP

#include <cstdint>
#include <string>
namespace forecast {

/**
 * @brief Contains all the possible errors returned by the methods of this
 * library.
 *
 */
enum class Status : uint8_t {
    NO_ERROR = 0,
    TICKER_INIT_ERR,         ///< Identify a wrong timer initialization
    MOTOR_ENCODER_INIT_ERR,  ///< Identify a wrong motor encoder initialization
    ENV_ENCODER_INIT_ERR,    ///< Identify a wrong environment encoder
                             ///< initialization
    ENV_MOTOR_ENCODER_INIT_ERR,    ///< Identify a wrong environment motor encoder
                             ///< initialization
    CONTROL_MOTOR_INIT_ERR,  ///< Identify a wrong control motor initialization
    ENV_MOTOR_INIT_ERR,      ///< Identify a wrong environment motor
                             ///< initialization
    TORQUE_SENSOR_INIT_ERR,  ///< Identify a wrong torque sensor initialization
    FREQ_NOT_KEPT,           ///< Identify that the loop frequency isn't kept
    COMMUNICATION_ERROR,     ///< Identify that a communication error is
                             ///< occurred
};

class Error {
    std::string str;

   public:
    Error() = default;

    inline void set(std::string msg){
        str = std::move(msg);
    }

    inline void reset() {
        str = "";
    }

    inline const std::string &msg() const {
        return str;
    }

    inline operator bool() const {
        return not str.empty();
    }

};
}  // namespace forecast

#endif  // HW_CMDS_HPP