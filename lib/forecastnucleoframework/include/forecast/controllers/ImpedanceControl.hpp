#ifndef IMPEDANCE_CONTROL_H
#define IMPEDANCE_CONTROL_H

#include <utility/filters/AnalogFilter.hpp>

#include "../Controller.hpp"

namespace forecast {

/**
 * @brief Impedance Control class
 **/
class ImpedanceControl : public Controller {
public:
  /**
   * @brief Construct a new ImpedanceControl object. This constructor
   initialize,
  * the controller.
  * @param kp
  * @param kd
  * @param k_des
  * @param b_des
  * @param j_des

  **/
  ImpedanceControl(float kp = 0, float kd = 0, float k_des = 0, float b_des = 0,
                   float j_des = 0);

  virtual float process(const IHardware *hw, std::vector<float> ref) override;

protected:
  float kp = 0.0;
  float kd = 0.0;

  float k_des = 0.0f;
  float b_des = 0.0f;
  float j_des = 0.0f;

  float tau_ref = 0.0f;
  float theta_eq = 0.0f;
  bool once = true;

  float tau = 0.0f;
  float dtau = 0.0f;
  float theta = 0.0f;
  float dtheta = 0.0f;
  float ddtheta = 0.0f;
  float dtheta_filt = 0.0f;
  float ddtheta_filt = 0.0f;

  float err = 0.0;
  float derr = 0.0;
  float dderr = 0.0;

  float errPast = 0.0;

  float out;

  utility::AnalogFilter *lowPassDTheta;
  utility::AnalogFilter *lowPassDDTheta;
};

inline ControllerFactory::Builder make_impedance_control_builder() {

  auto fn = [](std::vector<float> params) -> Controller * {
    if (params.size() < 5)
      return nullptr; // not enough parameters

    return new ImpedanceControl(params[0], params[1], params[2], params[3],
                                params[4]);
  };

  return {
      fn,
      {"Kp", "Kd", "K_des", "B_des", "J_des"},
      {"tau_err", "theta_eq", "err", "derr", "dtheta_filt", "ddtheta_filt"}};
}

} // namespace forecast

#endif // IMPEDANCE_CONTROL_H
