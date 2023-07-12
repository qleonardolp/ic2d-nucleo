#include <forecast/controllers/IntegralSlidingMode.hpp>
#include <debug.hpp>

#define __sign(x) (std::signbit(x) ? -1.0 : 1.0)

using namespace forecast;

IntegralSlidingModeForceControl::IntegralSlidingModeForceControl(float lambda1,
                                                                 float lambda2,
                                                                 float Jm,
                                                                 float Ks)
    : lambda1(lambda1), lambda2(lambda2), Ks(Ks), Jm(Jm), A(Jm / Ks),
      kp(A * lambda2 - 1), kd(A * lambda1), s(0.0), ni(0.0), phi(0.0), ka(0.0),
      err(0.0), ierr(0.0), derr(0.0), dderr(0.0), err_prev(0.0), 
      ddtheta_e(0.0) {
    // ntd

    logs.push_back(&err);
    logs.push_back(&derr);
    logs.push_back(&ierr);
}

float IntegralSlidingModeForceControl::process(const IHardware* hw,
                                               std::vector<float> reference) {

    auto ref = reference.size() < 1 ? 0 : reference[0];
    auto dref = reference.size() < 2 ? 0 : reference[1];
    auto ddref = reference.size() < 3 ? 0 : reference[2];

    err = hw->get_tau_m(0) - ref;
    derr = hw->get_d_tau_m(0) - dref;

    // SLIDING SURFACE
    if (fabs(s) > 2 * phi)
        ierr = -err / lambda1;  // to avoid the reaching phase
    else if (__sign(err) != __sign(err_prev))
        ierr = 0;
    ierr += err * hw->get_dt();
    s = derr + lambda1 * err + lambda2 * ierr;

    // CONTROL LAW
    us = __sign(s);
    if (fabs(s) < phi)
        us = s / phi;
    // ddtheta_e = ddtheta_m - ddtau / Ks;
    out =
        -kp * err - kd * derr + A * ddref + ref + ka * Jm * ddtheta_e - ni * us;

    err_prev = err;

    return out;
}

void IntegralSlidingModeForceControl::set_ddtheta(float ddtheta) {
    this->ddtheta_e = ddtheta;
}