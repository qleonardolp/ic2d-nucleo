#ifndef FORECAST_REFGEN_HPP
#define FORECAST_REFGEN_HPP

#include <vector>
#include <forecast/IHardware.hpp>

namespace forecast
{
    class ReferenceGenerator
    {
    public:
        /**
         * @brief   Reference Generator constructor
         */
        ReferenceGenerator() = default;

        /**
         * @brief Default virtual destructor
         *
         */
        virtual ~ReferenceGenerator() = default;

        /**
         * @brief   Implementation of the reference generator.
         *
         * @param   None
         */
        virtual std::vector<float> process(const IHardware *hw) = 0;
    };
}

#endif