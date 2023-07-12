#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <mbed.h>

#ifdef DEBUG_BUILD

#warning Debug serial communication is active. The software could be unable to maintain the specified frequency!

#ifdef TARGET_STM32F4
namespace __fc_dbg {
static Serial debug(PC_12, PD_2, 9600);
}
#endif

#ifdef TARGET_STM32L4
namespace __fc_dbg {
static Serial debug(D5, D4, 9600);
}
#endif

#define DEBUG_INFO(...) __fc_dbg::debug.printf(__VA_ARGS__)

#else

#define DEBUG_INFO(...)

#endif

#endif