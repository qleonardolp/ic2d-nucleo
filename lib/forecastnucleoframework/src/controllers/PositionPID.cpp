#include <forecast/controllers/PositionPID.hpp>

using namespace forecast;

PositionPID::PositionPID(float kp, float ki, float kd)
    : kp(kp), ki(ki), kd(kd), err(0.f), derr(0.f), ierr(0.f), errPast(0.f) {
  logs.push_back(&reference);

  lowPass = utility::AnalogFilter::getDifferentiatorHz(10.0f);
}

float PositionPID::process(const IHardware *hw, std::vector<float> ref) {
  theta = hw->get_theta(0);
  dtheta = hw->get_d_theta(0);
  reference = ref[0];

  err = ref[0] - theta;
  // err = 0.5;
  derr = (err - errPast) / hw->get_dt();
  ierr += err * hw->get_dt();

  errPast = err;

  out = kp * err + ki * ierr + kd * derr;
  return out;
}