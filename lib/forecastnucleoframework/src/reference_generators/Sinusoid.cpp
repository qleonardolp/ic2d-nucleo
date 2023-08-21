#include <forecast/reference_generators/Sinusoid.hpp>

forecast::SinusoidRefGen::SinusoidRefGen(float frequency, float amplitude) : frequency(frequency), amplitude(amplitude) {
    // ntd;
}

std::vector<float> forecast::SinusoidRefGen::process(const IHardware* hw) {
    float output = (amplitude * (-1)* cos(2 * M_PI * frequency * hw->get_current_time() )) + amplitude;
    return {output};
}
