#ifndef FORECAST_SINUSOID_REF_GEN_HPP
#define FORECAST_SINUSOID_REF_GEN_HPP

#include "../ReferenceGenerator.hpp"

namespace forecast {
    class SinusoidRefGen : public ReferenceGenerator {
       private:
        float frequency;
        float amplitude;
       public:

        SinusoidRefGen(float frequency = 1.0f, float amplitude = 1.0f);

        virtual std::vector<float> process(const IHardware* hw);
    };


    inline RefGenFactory::Builder make_sinusoid_ref_gen_builder() {

        auto fn = [](std::vector<float> params) -> ReferenceGenerator * {
            if (params.empty())
                return nullptr;
        
            return new SinusoidRefGen(params[0], params[1]);
        };

        // the empty string log name will be consider as a non valid log
        return {fn, {"Frequency, Amplitude"}};
    }
}

#endif // FORECAST_SINUSOID_REF_GEN_HPP
