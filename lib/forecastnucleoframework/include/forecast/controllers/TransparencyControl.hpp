#ifndef TRANSPARENCY_CONTROL_H
#define TRANSPARENCY_CONTROL_H

#include <utility/filters/AnalogFilter.hpp>

#include "../Controller.hpp"

namespace forecast {

/**
 * @brief Transparency Control class
 **/
class TransparencyControl : public Controller {
public:
  TransparencyControl(float kp = 0, float kd = 0);

  virtual float process(const IHardware *hw, std::vector<float> ref) override;

protected:
  float kp = 0.0;
  float kd = 0.0;

  float tau_ref = 0.0f;
  float theta = 0.0;

  float ddtheta_filt = 0.0f;
  float err = 0.0;
  float derr = 0.0;

  float errPast = 0.0;

  float out;

  utility::AnalogFilter *lowPass;
};

ControllerFactory::Builder make_Transparency_builder() {
  auto fn = [](std::vector<float> params) -> Controller * {
    if (params.size() < 2)
      return nullptr;

    return new TransparencyControl(params[0], params[1]);
  };

  return {fn, {"Kp", "Kd"}, {"err", "derr", "ierr"}};
}
} // namespace forecast

#endif // TRANSPARENCY_CONTROL_H
