#ifndef FORECAST_RAMP_REF_GEN_HPP
#define FORECAST_RAMP_REF_GEN_HPP

#include "../ReferenceGenerator.hpp"

namespace forecast {
    class RampRefGen : public ReferenceGenerator {
       private:
        float target_time;
        float amplitude;
       public:

        RampRefGen(float target_time = 1.0f, float amplitude = 1.0f);

        virtual std::vector<float> process(const IHardware* hw);
    };


    inline RefGenFactory::Builder make_ramp_ref_gen_builder() {

        auto fn = [](std::vector<float> params) -> ReferenceGenerator * {
            if (params.empty())
                return nullptr;
            return new RampRefGen(params[0], params[1]);
        };

        // the empty string log name will be consider as a non valid log
        return {fn, {"Target Time", "Max amplitude"}};
    }
}

#endif // FORECAST_CONSTANT_REF_GEN_HPP