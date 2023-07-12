#ifndef ADMITTANCE_CONTROL_H
#define ADMITTANCE_CONTROL_H

#include <utility/filters/AnalogFilter.hpp>

#include "../Controller.hpp"
#include <eigen.h>

namespace forecast {

/**
 * @brief Admittance control class
 **/
class AdmittanceControl : public Controller {
public:
  /**
   * @brief Admittance control constructor
   **/
  AdmittanceControl();
  /**
   * @brief Construct a new AdmittanceControl object. This constructor
  initialize,
   * the controller.
   * @param kp
   * @param kd
   * @param k_des
   * @param b_des
   * @param j_des

  **/
  AdmittanceControl(float kp, float kd, float k_des, float b_des, float j_des);

  /**
   * @brief Admittance Control process function
   * 
   * @param hw    Hardware instance
   * @param ref   Reference value
   * @return float Output
   */
  virtual float process(const IHardware *hw, std::vector<float> ref) override;


  /**
   * @brief Admittance Control process function (Eigen3 test)
   * 
   * @param hw    Hardware instance
   * @param ref   Reference value
   * @return float Output
   */
  float process_mtx(const IHardware *hw, std::vector<float> ref);

protected:
  float kp = 0.0f;
  float kd = 0.0f;

  float k_des = 0.0f;
  float b_des = 0.0f;
  float j_des = 0.0f;

  float tau = 0.0f;
  float dtau = 0.0f;
  float theta = 0.0f;
  float dtheta = 0.0f;
  float ddtheta = 0.0f;
  float dtheta_filt = 0.0f;
  float ddtheta_filt = 0.0f;

  float tau_err = 0.0f;
  float theta_ref = 0.0f;

  float err = 0.0f;
  float derr = 0.0f;
  float dderr = 0.0f;

  float err_past = 0.0;

  float out;

  utility::AnalogFilter *lowPassDTheta;
  utility::AnalogFilter *lowPassDDTheta;
  utility::AnalogFilter *admittanceTF;
};

/**
 * @brief Builder function for the add method of App class
 * 
 * @return ControllerFactory::Builder 
 */
inline ControllerFactory::Builder make_admittance_control_builder() {

  auto fn = [](std::vector<float> params) -> Controller * {
    if (params.size() < 5)
      return nullptr; // not enough parameters

    return new AdmittanceControl(params[0], params[1], params[2], params[3],
                                 params[4]);
  };

  return {
      fn,
      {"Kp", "Kd", "K_des", "B_des", "J_des"},
      {"tau_err", "theta_ref", "err", "derr", "dtheta_filt", "ddtheta_filt"}};
}
} // namespace forecast

#endif // ADMITTANCE_CONTROL_H
