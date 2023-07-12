#include <forecast/controllers/AdmittanceControl.hpp>

using namespace forecast;

AdmittanceControl::AdmittanceControl()
    : theta(0.f), err(0.f)
{
    lowPassDTheta = utility::AnalogFilter::getDifferentiatorHz(100.0f);
    lowPassDDTheta = utility::AnalogFilter::getDifferentiatorHz(10.0f);
}

AdmittanceControl::AdmittanceControl(float kp,
                                    float kd,
                                    float k_des,
                                    float b_des,
                                    float j_des)
    : kp(kp), kd(kd), k_des(k_des), b_des(b_des), j_des(j_des), theta(0.f),
      err(0.f)
{
    /*
        available logs for the user 
    */ 
    logs.push_back(&tau_err);
    logs.push_back(&theta_ref);
    logs.push_back(&err);
    logs.push_back(&derr);
    logs.push_back(&dtheta_filt);
    logs.push_back(&ddtheta_filt);
    
    lowPassDTheta = utility::AnalogFilter::getDifferentiatorHz(100.0f);
    lowPassDDTheta = utility::AnalogFilter::getDifferentiatorHz(10.0f);

    if(j_des > 0){
        double a_ADM[3] = {j_des,b_des,k_des};
        double b_ADM[3] = {0.0,0.0,1.0};   
        admittanceTF = new utility::AnalogFilter(2, a_ADM, b_ADM);
    }

    else {
        double a_ADM[2] = {b_des,k_des};
        double b_ADM[2] = {0.0,1.0};   
        admittanceTF = new utility::AnalogFilter(1, a_ADM, b_ADM); 
    }
}

float AdmittanceControl::process(const IHardware *hw, std::vector<float> ref)
{
    if (ref.size() < 1) // no ref, no output
        return 0.f;

    tau = -hw->get_tau_s(1); // tauSensor -> index 1
    dtau = -hw->get_d_tau_s(1);  // tauSensor

    theta = -hw->get_theta(0); // thetaM -> index 1
    dtheta = -hw->get_d_theta(0); 
    ddtheta = -hw->get_dd_theta(0);

    dtheta_filt = lowPassDTheta->process(theta, hw->get_dt());
    ddtheta_filt = lowPassDDTheta->process(dtheta, hw->get_dt());

    /* FORCE LOOP */
    tau_err = ref[0] - tau;
    theta_ref = admittanceTF->process(tau_err,hw->get_dt());

    /* POSITION LOOP */
    err = theta_ref - theta;
    derr = (err - err_past)/hw->get_dt();
    err_past = err;
    
    out = kp*err + kd*derr;

    return out;
}

float AdmittanceControl::process_mtx(const IHardware *hw, std::vector<float> ref)
{
    using namespace Eigen;

    // Def.:
    //        b0 s^3 + b1 s^2 + b2 s + b3
    // FT = --------------------------------, with a0 = 1!
    //        a0 s^3 + a1 s^2 + a2 s + a3
    // Forma Canonica Controlavel | (Ogata pg. 596):
    Matrix3f A; 
    A << 0, 1, 0, 0, 0, 1, -3, -2, -1;
    Vector3f B(0, 0, 1);
    RowVector3f C(3 - 3*0, 2 - 2*1, 1 - 1*2);
    float D = 0.2;
    // Discretizacao 2 Ord:
    float Ts = hw->get_dt();
    Matrix3f At_square = (A*Ts) * (A*Ts);
    Matrix3f Ak = Matrix3f::Identity() + A*Ts + At_square/2;
    Vector3f Bk = (Matrix3f::Identity() + A*Ts/2 + At_square/6)*B*Ts;

    tau_err = ref[0] - tau;
    Vector3f xk(tau_err, 0, 0);
    xk = Ak*xk + Bk*tau_err;
    float yk = C*xk + D*tau_err;

    return yk;
}