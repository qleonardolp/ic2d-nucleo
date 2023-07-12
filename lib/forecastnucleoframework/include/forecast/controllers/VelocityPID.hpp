#ifndef VELOCITY_PID_H
#define VELOCITY_PID_H

#include <utility/filters/AnalogFilter.hpp>
#include "../Controller.hpp"

namespace forecast
{
    /**
     * @brief VelocityPID control class
     **/

    class VelocityPID : public Controller
    {
    public:
        /**
         * @brief Construct a new VelocityPID object. This constructor initialize,
         * the controller.
         *
         * @param kp
         * @param ki
         * @param kd
         */
        VelocityPID(float kp, float ki, float kd);

        virtual float process(const IHardware *hw, std::vector<float> ref) override;

    protected:
        float kp = 0.0f;
        float ki = 0.0f;
        float kd = 0.0f;

        float theta = 0.0f;
        float dtheta = 0.0f;

        float err = 0.0f;
        float derr = 0.0f;
        float ierr = 0.0f;

        float errPast = 0.0f;

        float out;

        utility::AnalogFilter *lowPass;
    };

    inline ControllerFactory::Builder make_Velocity_PID_builder()
    {
        auto fn = [](std::vector<float> params) -> Controller *
        {
            if (params.size() < 2)
                return nullptr;

            return new VelocityPID(params[0], params[1], params[2]);
        };

        // the empty string log name will be consider as a non valid log
        return {fn, {"Kp", "Ki", "Kd"}, {"err", "derr", "ierr"}};
    }

} // namespace forecast

#endif // VELOCITY_PID_H