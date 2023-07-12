#ifndef FORECAST_SWEEP_REF_GEN_HPP
#define FORECAST_SWEEP_REF_GEN_HPP

#include "../ReferenceGenerator.hpp"

namespace forecast
{
    class SweepRefGen : public ReferenceGenerator
    {
    private:
        float max_frequency;
        float duration;
        float amplitude;

    public:
        SweepRefGen(float max_frequency, float duration, float amplitude);
        virtual std::vector<float> process(const IHardware *hw);
    };

    inline RefGenFactory::Builder make_sweep_ref_gen_builder()
    {
        auto fn = [](std::vector<float> params) -> ReferenceGenerator *
        {
            if (params.empty())
                return nullptr;

            return new SweepRefGen(params[0], params[1], params[2]);
        };
        return {fn, {"Max frequency, Duration, Amplitude"}};
    }
}

#endif
