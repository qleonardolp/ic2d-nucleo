#ifndef MEGA_PD_H
#define MEGA_PD_H

#include "../Controller.hpp"

namespace forecast {
/**
 * @brief MegaPD control class
 **/
class MegaPD : public Controller {
   public:
    MegaPD(float kp=0, float kd=0, float ff=0);

    virtual float process(const IHardware* hw, std::vector<float> ref) override;

   protected:
    float kp = 0.0;
    float kd = 0.0;
    float ff = 0.0;
    float err = 0.0;
    float derr = 0.0;
    float out;
};

inline ControllerFactory::Builder make_mega_PD_builder() {

    auto fn = [](std::vector<float> params) -> Controller * {
        if (params.size() < 3)
        return nullptr;
    
        return new MegaPD(params[0], params[1], params[2]);
    };

    return {fn, {"KP", "KD", "FF"}, {"err", "derr"}};
}

}  // namespace forecast
#endif  // MEGA_PD_H