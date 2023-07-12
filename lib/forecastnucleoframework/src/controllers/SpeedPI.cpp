#include <forecast/controllers/SpeedPI.hpp>

using namespace forecast;

#define __sign(x) (std::signbit(x) ? -1.0 : 1.0)

SpeedPI::SpeedPI(float kp, float ki,
        float pos_a0, float pos_a1, float pos_a2, float pos_a3,
        float pos_a4, float pos_a5, float pos_a6, float pos_a7,
        float pos_a8, float pos_b1, float pos_b2, float pos_b3,
        float pos_b4, float pos_b5, float pos_b6, float pos_b7,
        float pos_b8, float pos_w,
        float neg_a0, float neg_a1, float neg_a2, float neg_a3,
        float neg_a4, float neg_a5, float neg_a6, float neg_a7,
        float neg_a8, float neg_b1, float neg_b2, float neg_b3,
        float neg_b4, float neg_b5, float neg_b6, float neg_b7,
        float neg_b8, float neg_w, float compensation_active)
    : kp(kp),
      ki(ki),
      pos_a0(pos_a0),
      pos_a1(pos_a1),
      pos_a2(pos_a2),
      pos_a3(pos_a3),
      pos_a4(pos_a4),
      pos_a5(pos_a5),
      pos_a6(pos_a6),
      pos_a7(pos_a7),
      pos_a8(pos_a8),
      pos_b1(pos_b1),
      pos_b2(pos_b2),
      pos_b3(pos_b3),
      pos_b4(pos_b4),
      pos_b5(pos_b5),
      pos_b6(pos_b6),
      pos_b7(pos_b7),
      pos_b8(pos_b8),
      pos_w(pos_w),
      neg_a0(neg_a0),
      neg_a1(neg_a1),
      neg_a2(neg_a2),
      neg_a3(neg_a3),
      neg_a4(neg_a4),
      neg_a5(neg_a5),
      neg_a6(neg_a6),
      neg_a7(neg_a7),
      neg_a8(neg_a8),
      neg_b1(neg_b1),
      neg_b2(neg_b2),
      neg_b3(neg_b3),
      neg_b4(neg_b4),
      neg_b5(neg_b5),
      neg_b6(neg_b6),
      neg_b7(neg_b7),
      neg_b8(neg_b8),
      neg_w(neg_w),
      compensation_active(compensation_active),
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

float SpeedPI::process(const IHardware *hw, std::vector<float> ref)
{
    theta = hw->get_theta(1); // hw->getThetaE();
    dtheta = hw->get_d_theta(1); // hw->getDThetaE();

    err = ref[0] - dtheta; // TODO è giusto 0??
    derr = (err-errPast) / hw->get_dt();
    ierr += err * hw->get_dt();

    errPast = err;

    out = kp*err + ki*ierr;

    if(compensation_active == 0)
        return out;

    out = 0;

    // // taue
    float pos_viscoso = 0.0118;
    // float pos_a0 = 0.0279;
    // float pos_a1 = 0.0054;
    // float pos_b1 = -0.0158;
    // float pos_a2 = -0.0008;
    // float pos_b2 = -0.0017;
    // float pos_a3 = -0.0000;
    // float pos_b3 = -0.0011;
    // float pos_a4 = -0.0008;
    // float pos_b4 = -0.0011;
    // float pos_a5 = -0.0003;
    // float pos_b5 = 0.0003;
    // float pos_a6 = -0.0013;
    // float pos_b6 = 0.0009;
    // float pos_a7 = -0.0006;
    // float pos_b7 = -0.0001;
    // float pos_a8 = -0.0001;
    // float pos_b8 = -0.0005;
    // float pos_w = 1;

    float neg_viscoso = 0.0083;
    // float neg_a0 = -0.0714;
    // float neg_a1 = -0.0089;
    // float neg_b1 = -0.0152;
    // float neg_a2 = -0.0008;
    // float neg_b2 = 0.0002;
    // float neg_a3 = 0.0011;
    // float neg_b3 = 0.0031;
    // float neg_a4 = 0.0003;
    // float neg_b4 = 0.0013;
    // float neg_a5 = 0.0003;
    // float neg_b5 = -0.0005;
    // float neg_a6 = 0.0004;
    // float neg_b6 = -0.0001;
    // float neg_a7 = 0.0003;
    // float neg_b7 = -0.0001;
    // float neg_a8 = -0.0005;
    // float neg_b8 = -0.0008;
    // float neg_w = 1;

    float comp = 0;

    if(dtheta < -0.05){
        comp =  (
            -dtheta * neg_viscoso +
            neg_a0 + neg_a1*cos(theta*neg_w) + neg_b1*sin(theta*neg_w) +
            neg_a2*cos(2*theta*neg_w) + neg_b2*sin(2*theta*neg_w) +
            neg_a3*cos(3*theta*neg_w) + neg_b3*sin(3*theta*neg_w) +
            neg_a4*cos(4*theta*neg_w) + neg_b4*sin(4*theta*neg_w) +
            neg_a5*cos(5*theta*neg_w) + neg_b5*sin(5*theta*neg_w) +
            neg_a6*cos(6*theta*neg_w) + neg_b6*sin(6*theta*neg_w) +
            neg_a7*cos(7*theta*neg_w) + neg_b7*sin(7*theta*neg_w) +
            neg_a8*cos(8*theta*neg_w) + neg_b8*sin(8*theta*neg_w)
        );

        out += comp;
    } else if(dtheta > 0.05) {
        comp = (
            dtheta * pos_viscoso +
            pos_a0 + pos_a1*cos(theta*pos_w) + pos_b1*sin(theta*pos_w) +
            pos_a2*cos(2*theta*pos_w) + pos_b2*sin(2*theta*pos_w) +
            pos_a3*cos(3*theta*pos_w) + pos_b3*sin(3*theta*pos_w) +
            pos_a4*cos(4*theta*pos_w) + pos_b4*sin(4*theta*pos_w) +
            pos_a5*cos(5*theta*pos_w) + pos_b5*sin(5*theta*pos_w) +
            pos_a6*cos(6*theta*pos_w) + pos_b6*sin(6*theta*pos_w) +
            pos_a7*cos(7*theta*pos_w) + pos_b7*sin(7*theta*pos_w) +
            pos_a8*cos(8*theta*pos_w) + pos_b8*sin(8*theta*pos_w)
        );

        out += comp;
    }

    return out;
}
