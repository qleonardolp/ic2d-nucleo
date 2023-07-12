#include <forecast/controllers/EnvRenderingControl.hpp>

using namespace forecast;

EnvRenderingControl::EnvRenderingControl(float k_des,
                                         float b_des,
                                         float j_des)
    : 
      k_des(k_des),
      b_des(b_des),
      j_des(j_des),
      out(0.0)
{
    lowPassDTheta = utility::AnalogFilter::getDifferentiatorHz(100.0f);
    lowPassDDTheta = utility::AnalogFilter::getDifferentiatorHz(10.0f);

    is_admittance_control = j_des >= 0.01;

    /***** ADMITTANCE CONTROL *****/
    if(is_admittance_control) {
        if(j_des > 0){
            double a_ADM[3] = {j_des,b_des,k_des};
            double b_ADM[3] = {0.0,0.0,1.0};   
            admittanceTF = new utility::AnalogFilter(2, a_ADM, b_ADM);
        } else {
            double a_ADM[2] = {b_des,k_des};
            double b_ADM[2] = {0.0,1.0};   
            admittanceTF = new utility::AnalogFilter(1, a_ADM, b_ADM); 
        }
    } 
    /****** IMPEDANCE CONTROL ****/
    else {
        once = true;
    }

    logs.push_back(&dtheta_filt);
    logs.push_back(&ddtheta_filt);  
    logs.push_back(&tau_err);
    logs.push_back(&theta_ref);
    logs.push_back(&err);
    logs.push_back(&derr);
}

float EnvRenderingControl::process(const IHardware *hw, std::vector<float> ref)
{
    tau = -hw->get_tau_s(1);
    dtau = -hw->get_d_tau_s(1);
    theta = -hw->get_theta(2);
    dtheta = -hw->get_d_theta(2);
    ddtheta = -hw->get_dd_theta(2);
    // dtheta_filt = lowPassDTheta->process(theta, hw->get_dt());
    ddtheta_filt = lowPassDDTheta->process(dtheta, hw->get_dt());

    /***** ADMITTANCE CONTROL *****/
    if(is_admittance_control) {
        /* FORCE LOOP */
        tau_err = ref[0] - tau;
        theta_ref = admittanceTF->process(tau_err,hw->get_dt());

        /* POSITION LOOP */
        err = theta_ref - theta;
        derr = (err - err_past)/hw->get_dt();
        err_past = err;
        
        out = kp_adm*err + kd_adm*derr;
    }

    /****** IMPEDANCE CONTROL ****/
    else {
        /* Get the equilibrium state */
        if (once)
        {
            theta_eq = theta;
            once = false;
            err_past = 0;
        }

        /* POSITION LOOP */
        tau_ref = ref[0] - k_des * (theta - theta_eq) - b_des *
                  dtheta_filt - j_des * ddtheta_filt;

        /* FORCE LOOP */
        err = tau_ref - tau;
        derr = (err - err_past) / hw->get_dt();
        err_past = err;

        out = tau_ref + kp_imp*err + kd_imp*derr;
    }

    return out;
}