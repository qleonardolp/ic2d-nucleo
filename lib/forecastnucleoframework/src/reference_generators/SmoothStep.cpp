#include <forecast/reference_generators/SmoothStep.hpp>
#include <utility/math.hpp>

forecast::SmoothStepRefGen::SmoothStepRefGen(float amplitude) : amplitude(amplitude) {
    // ntd;
}

std::vector<float> forecast::SmoothStepRefGen::process(const IHardware* hw) {
    float time = hw->get_current_time();
    float output = time*time*utility::sign(amplitude);
    if (abs(output) > abs(amplitude))
        output = amplitude;
    return {output};
}
