#ifndef FORECAST_CONSTANT_REF_GEN_HPP
#define FORECAST_CONSTANT_REF_GEN_HPP

#include "../ReferenceGenerator.hpp"

namespace forecast {
    class ConstantRefGen : public ReferenceGenerator {
       private:
        std::vector<float> values;
       public:

        ConstantRefGen(std::vector<float> v = {0.f});

        virtual std::vector<float> process(const IHardware* hw);
    };


    inline RefGenFactory::Builder make_constant_ref_gen_builder() {

        auto fn = [](std::vector<float> params) -> ReferenceGenerator * {
            if (params.empty())
                return nullptr;
        
            return new ConstantRefGen({params[0]});
        };

        // the empty string log name will be consider as a non valid log
        return {fn, {"constant value"}};
    }
}

#endif // FORECAST_CONSTANT_REF_GEN_HPP