#include <forecast/reference_generators/Sweep.hpp>

forecast::SweepRefGen::SweepRefGen(float max_freq, float duration, float amplitude) : max_frequency(max_freq),duration(duration),amplitude(amplitude) {
    // ntd;
}

std::vector<float> forecast::SweepRefGen::process(const IHardware* hw) {
    float time = hw->get_current_time();
    float frequency = max_frequency/(2*duration) * time;
    float output = amplitude * sin(2*M_PI*time * frequency);
    return {output};
}
