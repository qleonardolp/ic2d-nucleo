#include <forecast/controllers/VelocityPID.hpp>

using namespace forecast;

VelocityPID::VelocityPID(float kp, float ki, float kd)
    : kp(kp),
      ki(ki),
      kd(kd),
      err(0.f),
      derr(0.f),
      ierr(0.f),
      errPast(0.f)
{
    logs.push_back(&err);
    logs.push_back(&derr);
    logs.push_back(&ierr);

    lowPass = utility::AnalogFilter::getDifferentiatorHz(20.0f);
}

float VelocityPID::process(const IHardware *hw, std::vector<float> ref)
{
    theta = hw->get_theta(0);
    dtheta = hw->get_d_theta(0);

    err = ref[0] - dtheta;
    derr = (err - errPast) / hw->get_dt();
    ierr += err * hw->get_dt();

    errPast = err;

    out = kp * err + ki * ierr + kd * derr;

    return out;
}
