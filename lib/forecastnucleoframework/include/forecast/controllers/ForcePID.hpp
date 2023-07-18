#ifndef FORCE_PID_H
#define FORCE_PID_H

#include <utility/filters/AnalogFilter.hpp>
#include "../Controller.hpp"

namespace forecast {

/**
 * @brief ForcePID control class
 **/

class ForcePID : public Controller {
   public:

    /**
     * @brief Construct a new Force P I D object. This constructor initialize,
     * the controller.
     *
     * @param kp
     * @param ki
     * @param kd
     */
    ForcePID(float kp = 0, float ki = 0, float kd = 0);

    virtual float process(const IHardware* hw, std::vector<float> ref) override;

   protected:
    float kp = 0.0;
    float ki = 0.0;
    float kd = 0.0;

    float tau = 0.0f;
    float dtau = 0.0f;

    float err = 0.0;
    float derr = 0.0;
    float ierr = 0.0;

    float errPast = 0.0;

    float out;
    float reference = 0.0;

    utility::AnalogFilter* lowPass;
    utility::AnalogFilter* lowPassD;
};

inline ControllerFactory::Builder make_Force_PID_builder() {

    auto fn = [](std::vector<float> params) -> Controller * {
        if (params.size() < 2)
            return nullptr;
        return new ForcePID(params[0], params[1], params[2]);
    };

    return {fn, {"KP", "KI", "KD"}, {"reference"}};
}

}  // namespace forecast

#endif  // FORCE_PID_H