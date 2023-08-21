#include <forecast/reference_generators/ConstantRefGen.hpp>

forecast::ConstantRefGen::ConstantRefGen(std::vector<float> v) : values(std::move(v)) {
    // ntd;
}

std::vector<float> forecast::ConstantRefGen::process(const IHardware* hw) {
    float time = hw->get_current_time();
    float duration = hw->get_duration();

if (time > 2.0f && time < (duration - 2)) {
        return values;
    } 
    else {
        return {0};
    }
}