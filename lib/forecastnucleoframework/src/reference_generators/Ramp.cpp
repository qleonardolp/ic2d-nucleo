#include <forecast/reference_generators/Ramp.hpp>
#include <utility/math.hpp>

forecast::RampRefGen::RampRefGen(float target_time, float amplitude) : target_time(target_time), amplitude(amplitude) {
    // ntd;
}

std::vector<float> forecast::RampRefGen::process(const IHardware* hw) {    
    float time = hw->get_current_time();
    float output = amplitude/target_time*time*utility::sign(amplitude);
    if (abs(output) >= abs(amplitude))
        output = amplitude;
   return {output};
}
