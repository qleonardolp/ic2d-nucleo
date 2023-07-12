#include <forecast/reference_generators/ConstantRefGen.hpp>

forecast::ConstantRefGen::ConstantRefGen(std::vector<float> v) : values(std::move(v)) {
    // ntd;
}

std::vector<float> forecast::ConstantRefGen::process(const IHardware* hw) {
    return values;
}
