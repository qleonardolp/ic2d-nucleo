#ifndef SLIDING_MODE_CONTROL_H
#define SLIDING_MODE_CONTROL_H

#include "../Controller.hpp"

namespace forecast {
class IntegralSlidingModeForceControl : public Controller {
   public:

    /**
     * @brief Construct a new Integral Sliding Mode object. This constructor
     * initialize, the controller.
     *
     * @param lambda1
     * @param lambda2
     * @param Jm
     * @param Ks
     */
    IntegralSlidingModeForceControl(float lambda1 = 0,
                                    float lambda2 = 0,
                                    float Jm = 0,
                                    float Ks = 0);

    virtual float process(const IHardware* hw, std::vector<float> ref) override;

    void set_ddtheta(float ddtheta);  // Read from the accelerometer

   protected:
    float lambda1, lambda2;
    float Ks, Jm, A;
    float kp, kd;  // TO READ
    float s;
    float ni, phi, ka;  // TO SET
    float err, ierr, derr, dderr, err_prev;
    float ddtheta_e;  // Required for acceleration feedback
    float us;
    float out;
};


inline ControllerFactory::Builder make_integral_sliding_mode_builder() {

    auto fn = [](std::vector<float> params) -> Controller *{
        if (params.size() < 4) 
            return nullptr; // not enough parameters
        
        return new IntegralSlidingModeForceControl(params[0], params[1], 
                                                   params[2], params[3]);
    };

    return {fn, {"lambda 1","lambda 2","Ks", "Jm"}, {"err", "derr", "ierr"}};
}

}  // namespace forecast

#endif  // SLIDING_MODE_CONTROL_H
