#include <forecast/controllers/Bypass.hpp>

using namespace forecast;

Bypass::Bypass() {
  logs.push_back(&reference);
}

float Bypass::process(const IHardware *hw, std::vector<float> ref) {
  reference = ref[0];
  return reference;
}