#ifndef FORECAST_COM_HPP
#define FORECAST_COM_HPP

#include <stdint.h>

namespace forecast {
namespace com {

constexpr uint8_t stx = 0x02u;
constexpr uint8_t etx = 0x03u;
constexpr uint8_t esc = 0x1bu;

enum Type : uint8_t {
    // answers
    ACK = 0x06u,            /**< answer when the previous operation is been
                                 completed */
    NACK = 0x0fu,           ///< answer when the previous operation was failed

    // requests
    REQ_HW_DATA = 0x20u,    /**< hw data O

                                 Answer payload:
                                 * list of motors names separated by commas
                                 * a '\t' char
                                 * list of available hardware data for logging
                            */

    REQ_VER,                /**< firmware version I

                                 Answer payload:
                                 * string with the semantic version of the FW
                            */

    REQ_CTRL_LIST,          /**< available controllers I

                                 Answer payload:
                                 * comma separated list of available controllers
                            */

    REQ_REF_GEN_LIST,       /**< available reference generator functions I

                                 Answer payload:
                                 * comma separated list of available reference
                                   generators
                            */

    REQ_CTRL_PARAMS,        /**< Controller parameters I

                                 Input payload:
                                 * string with the controller id

                                 Answer payload:
                                 * comma separated list of parameters
                            */
    REQ_REF_GEN_PARAMS,     /**< Reference generator's parameters I

                                 Input payload:
                                 * string with the reference generator id

                                 Answer payload:
                                 * comma separated list of parameters
                            */
    REQ_CTRL_LOGS,          /**< Controller loggable variables I

                                 Input payload:
                                 * string with the controller id

                                 Answer payload:
                                 * comma separated list of loggable variables
                            */
    REQ_READY,              /**< Ready I?

                                 Answer payload:
                                 * byte with a number greater then 0 if the
                                   program is ready to run the next task 0
                                   otherwise
                            */

    // commands
    CMD_SET_CTRL = 0x40u,   /**< Set a controller for a motor O

                                 Input payload:
                                 * null terminated string with the controller
                                   name
                                 * null terminated string with the motor name
                                 * list of float parameters for the controller
                              */
    CMD_SET_HW_LOGS,           /**< Set the hw variables to be logged every
                                 cycle

                                 Input payload:
                                 * comma separated list of hw variables
                              */

    CMD_SET_CTRL_LOGS,      /*< Set the controller logs to be loged every cycle

                                Input payload:
                                * uint8_t with the motor index
                                *comma separeted list of log names
                              */
    CMD_SET_REF_GEN,        /**< Set a reference generator I
                                 Input payload:
                                 * null terminated string with the reference
                                   generator name
                                 * null terminated string with the motor name
                                 * list of float parameters for the controller
                            */
    CMD_START_LOOP,         /**< Start the controll loop for the task  I

                                 Input payload:
                                 * float with the frequency of the loop in Hz
                                 * float with the duration in seconds
                            */
    CMD_STANDBY,            ///< Go to the stand by state I

    // no answer
    LOGS = 0x60u,           /**< This message contains logs from the task

                                 Output payload:
                                 * list of floats

                                 N.B. this specific message does not require
                                 an reply message.
                            */
    ERROR,                  /**< This message contains an error message

                                 Output payload:
                                 * null terminated string with the error message

                                 N.B. this specific message does not require
                                 an reply message.
                            */
     END_LOG, 
};

enum err {
    UNESCAPED_CHAR = -1,
};

uint32_t escape(const void* src, void* dst, uint32_t srcSz, uint32_t maxSz = 0);

uint32_t unescape(const void* src,
                  void* dst,
                  uint32_t srcSz,
                  uint32_t maxSz = 0);

uint8_t checksum(const uint8_t* src, uint32_t sz);

uint32_t buildPkg(com::Type t,
                  const void* payload,
                  void* dst,
                  uint32_t payloadSz,
                  uint32_t maxSz = 0);

bool checkEtx(const uint8_t* pkg, uint32_t size);

}  // namespace com
}  // namespace forecast

#endif
