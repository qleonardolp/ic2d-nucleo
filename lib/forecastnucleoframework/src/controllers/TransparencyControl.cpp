#include <forecast/controllers/TransparencyControl.hpp>

using namespace forecast;

TransparencyControl::TransparencyControl(float kp,float kd): kp(kp), kd(kd),
                                                             theta(0.f), 
                                                             err(0.f) {
    
    logs.push_back(&err);
    logs.push_back(&derr);

    lowPass = utility::AnalogFilter::getDifferentiatorHz(30.0f);
}

float TransparencyControl::process(const IHardware* hw, std::vector<float> ref){

    err = ref[0] - (-hw->get_theta(1));
    // err = ref - (-hw->get_tau_m(0));
    derr = (err - errPast) / hw->get_dt();

    errPast = err;

    out = kp * err + kd * derr;

    return out;
}