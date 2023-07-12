#include <forecast/controllers/ForcePID.hpp>

using namespace forecast;

ForcePID::ForcePID(float kp, float ki, float kd)
    : kp(kp),
      ki(ki),
      kd(kd),
      errPast(0.f),
      err(0.f),
      derr(0.f),
      ierr(0.f)
{
    logs.push_back(&reference);
    lowPass = utility::AnalogFilter::getDifferentiatorHz(10.0f);
}

float ForcePID::process(const IHardware *hw, std::vector<float> ref)
{
    tau = hw->get_tau_s(0);
    dtau = hw->get_d_tau_s(0);
    reference = ref[0];

    err = ref[0] - tau;
    derr = (err - errPast) / hw->get_dt();
    ierr += err * hw->get_dt();
    errPast = err;

    out = ref[0] + kp * err + kd * derr + ki * ierr;

    return out;
}