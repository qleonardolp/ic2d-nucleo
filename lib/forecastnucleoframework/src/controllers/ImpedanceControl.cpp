#include <forecast/controllers/ImpedanceControl.hpp>

using namespace forecast;

ImpedanceControl::ImpedanceControl(float kp,
                                   float kd,
                                   float k_des,
                                   float b_des,
                                   float j_des)
    : theta_eq(0.f),
      kp(kp),
      kd(kd),
      k_des(k_des),
      b_des(b_des),
      j_des(j_des),
      theta(0.f),
      err(0.f),
      once(true)
{
    lowPassDTheta = utility::AnalogFilter::getDifferentiatorHz(100.0f);
    lowPassDDTheta = utility::AnalogFilter::getDifferentiatorHz(10.0f);

    logs.push_back(&tau_ref);
    logs.push_back(&theta_eq);
    logs.push_back(&err);
    logs.push_back(&derr);
    logs.push_back(&dtheta_filt);
    logs.push_back(&ddtheta_filt);
}

float ImpedanceControl::process(const IHardware *hw, std::vector<float> ref)
{

    tau = -hw->get_tau_s(1);
    dtau = -hw->get_d_tau_s(1);
    theta = -hw->get_theta(0);
    dtheta = -hw->get_d_theta(0);
    ddtheta = -hw->get_dd_theta(0);
    dtheta_filt = lowPassDTheta->process(theta, hw->get_dt());
    ddtheta_filt = lowPassDDTheta->process(dtheta, hw->get_dt());

    /* Get the equilibrium state */
    if (once)
    {
        theta_eq = theta;
        once = false;
        errPast = 0;
    }

    /* POSITION LOOP */
    tau_ref = ref[0] - k_des * (theta - theta_eq) - b_des * dtheta_filt - j_des * ddtheta_filt;

    /* FORCE LOOP */
    err = tau_ref - tau;
    derr = (err - errPast) / hw->get_dt();
    errPast = err;

    out = tau_ref + kp * err + kd * derr;

    return out;
}