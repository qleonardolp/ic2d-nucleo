#ifndef FORECAST_SMOOTHSTEP_REF_GEN_HPP
#define FORECAST_SMOOTHSTEP_REF_GEN_HPP

#include "../ReferenceGenerator.hpp"

namespace forecast {
    class SmoothStepRefGen : public ReferenceGenerator {
       private:
        float amplitude;
       public:

        SmoothStepRefGen(float amplitude = 1.0f);

        virtual std::vector<float> process(const IHardware* hw);
    };


    inline RefGenFactory::Builder make_smoothstep_ref_gen_builder() {

        auto fn = [](std::vector<float> params) -> ReferenceGenerator * {
            if (params.empty())
                return nullptr;
        
            return new SmoothStepRefGen(params[0]);
        };

        // the empty string log name will be consider as a non valid log
        return {fn, {"Amplitude"}};
    }
}

#endif // FORECAST_CONSTANT_REF_GEN_HPP
