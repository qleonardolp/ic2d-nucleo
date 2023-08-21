#include <forecast/reference_generators/Sinusoid.hpp>

forecast::SinusoidRefGen::SinusoidRefGen(float frequency, float amplitude) : frequency(frequency), amplitude(amplitude) {
    // ntd;
}

std::vector<float> forecast::SinusoidRefGen::process(const IHardware* hw) {
    float output = (amplitude * sin(2 * M_PI * frequency * hw->get_current_time() ));
    return {output};
}
