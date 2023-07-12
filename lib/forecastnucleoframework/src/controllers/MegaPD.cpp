#include <forecast/controllers/MegaPD.hpp>

using namespace forecast;

MegaPD::MegaPD(float kp, float kd, float ff)
    : kp(kp), kd(kd), ff(ff), err(0.f), derr(0.f) {
    logs.push_back(&err);
    logs.push_back(&derr);
}

float MegaPD::process(const IHardware* hw, std::vector<float> reference) {
    auto ref = reference.empty() ? 0.f : reference[0];
    auto dref = reference.size() < 2 ? 0.f : reference[1];

    err = ref - hw->get_tau_s(0);
    derr = dref - hw->get_d_tau_s(0);

    out = ff * ref + err * kp + derr * kd;

    return out;
}
