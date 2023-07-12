#ifndef UTILITY_MATH_HPP
#define UTILITY_MATH_HPP

#include "filters/Filter.hpp"
#include "filters/AnalogFilter.hpp"
#include "filters/DigitalFilter.hpp"

#include <cstdint>

namespace utility {

    struct ddvar {
        float val = 0.0;
        float dval = 0.0;
        float ddval = 0.0;
    };

    template<typename T>
    constexpr int8_t sign(T x) {return (x < 0.) ? -1 : 1;}

    template<typename T>
    constexpr T d_dt(T x, T prev_x, T dt) {return (x - prev_x) / dt;}
}

#endif // UTILITY_MATH_HPP