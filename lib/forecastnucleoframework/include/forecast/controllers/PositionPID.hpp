#ifndef POSITION_PID_H
#define POSITION_PID_H

#include <utility/filters/AnalogFilter.hpp>
#include "../Controller.hpp"

namespace forecast
{
    /**
     * @brief PositionPID control class
     **/

    class PositionPID : public Controller
    {
    public:
        /**
         * @brief Construct a new Position P I D object. This constructor initialize,
         * the controller.
         *
         * @param kp
         * @param ki
         * @param kd
         */
        PositionPID(float kp = 0.0f, float ki = 0.0f, float kd = 0.0f);

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
        float reference = 0.0;

        utility::AnalogFilter *lowPass;
    };

    inline ControllerFactory::Builder make_Position_PID_builder()
    {
        auto fn = [](std::vector<float> params) -> Controller *
        {
            if (params.size() < 2)
                return nullptr;

            return new PositionPID(params[0], params[1], params[2]);
        };

        return {fn, {"Kp", "Ki", "Kd"}, {"Reference"}};
    }

} // namespace forecast

#endif // POSITION_PID_H