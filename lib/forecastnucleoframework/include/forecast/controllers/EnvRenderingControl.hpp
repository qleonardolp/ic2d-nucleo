#ifndef ENV_RENDERING_CONTROL_H
#define ENV_RENDERING_CONTROL_H

#include "../Controller.hpp"
#include <utility/filters/AnalogFilter.hpp>

namespace forecast {

/**
 * @brief Environment Rendering Control class
 **/
class EnvRenderingControl : public Controller {
   public:
    /**
     * @brief Environment Rendering Control constructor
     **/
    EnvRenderingControl();
    /**
     * @brief Construct a new EnvRenderingControl object. This constructor
    initialize,
     * the controller.
     * @param k_des
     * @param b_des
     * @param j_des

    **/
    EnvRenderingControl(float k_des, float b_des, float j_des);

    virtual float process(const IHardware* hw, std::vector<float> ref) override;

   protected:

    float k_des = 0.0f;
    float b_des = 0.0f;
    float j_des = 0.0f;

    float damping_factor = 0.0f; 
    float tau_ref = 0.0f;
    
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
    float err_past = 0.0;
    utility::AnalogFilter *lowPassDTheta;
    utility::AnalogFilter *lowPassDDTheta;

    bool is_admittance_control = true;

    // ADMITTANCE PARAMETERS
    float kp_adm = 130;
    float kd_adm = 2.0f;
    float theta_ref = 0.0f;
    float tau_err = 0.0f;
    utility::AnalogFilter* admittanceTF;

    // IMPEDANCE PARAMETERS
    float kp_imp = 8.0f;
    float kd_imp = 0.02f;
    bool once = true;
    float theta_eq = 0.0f;

    float out;
};


inline ControllerFactory::Builder make_EnvRenderingController_damped() {
    auto fn = [](std::vector<float> params) -> Controller *{
        if (params.size() < 3)
                return nullptr;

            auto b_des = params[1] * 2.0f * sqrt(params[0] * params[2]);

            return new EnvRenderingControl(params[0], b_des, params[2]);
    };

    return {fn, {"K_e_vect", "DAMPING_FACTOR", "J_e_vect"}, 
            {"dtheta_filt", "ddtheta_filt", "tau_err", "theta_ref", "err", 
            "derr"}};

};

inline ControllerFactory::Builder make_EnvRenderingController() {
    auto fn = [](std::vector<float> params) -> Controller *{
        if (params.size() < 3)
                return nullptr;

            return new EnvRenderingControl(params[0], params[1], params[2]);
    };

    return {fn, {"K_e_vect", "b_des", "J_e_vect"}, 
            {"dtheta_filt", "ddtheta_filt", "tau_err", "theta_ref", "err", 
            "derr"}};

};
}  // namespace forecast

#endif  // ENV_RENDERING_CONTROL_H
