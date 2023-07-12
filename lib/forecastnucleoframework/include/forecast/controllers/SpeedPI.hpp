#ifndef SPEED_PI_H
#define SPEED_PI_H

#include <utility/filters/AnalogFilter.hpp>
#include "../Controller.hpp"

namespace forecast {

/**
 * @brief SpeedPI control class
 **/

class SpeedPI : public Controller {
   public:
    /**
     * @brief Construct a new SpeedPI object. This constructor initialize,
     * the controller.
     *
     * @param kp
     * @param ki
     * @param pos_an
     * @param pos_bn
     * @param pos_w
     * @param neg_an
     * @param neg_bn
     * @param neg_w
     * @param compensation_active
     */
    SpeedPI(float kp, float ki,
        float pos_a0, float pos_a1, float pos_a2, float pos_a3,
        float pos_a4, float pos_a5, float pos_a6, float pos_a7,
        float pos_a8, float pos_b1, float pos_b2, float pos_b3,
        float pos_b4, float pos_b5, float pos_b6, float pos_b7,
        float pos_b8, float pos_w,
        float neg_a0, float neg_a1, float neg_a2, float neg_a3,
        float neg_a4, float neg_a5, float neg_a6, float neg_a7,
        float neg_a8, float neg_b1, float neg_b2, float neg_b3,
        float neg_b4, float neg_b5, float neg_b6, float neg_b7,
        float neg_b8, float neg_w, float compensation_active);

    virtual float process(const IHardware* hw, std::vector<float> ref) override;

   protected:
    float kp = 0.0;
    float ki = 0.0;

    float pos_a0 = 0.0;
    float pos_a1 = 0.0;
    float pos_a2 = 0.0;
    float pos_a3 = 0.0;
    float pos_a4 = 0.0;
    float pos_a5 = 0.0;
    float pos_a6 = 0.0;
    float pos_a7 = 0.0;
    float pos_a8 = 0.0;

    float pos_b1 = 0.0;
    float pos_b2 = 0.0;
    float pos_b3 = 0.0;
    float pos_b4 = 0.0;
    float pos_b5 = 0.0;
    float pos_b6 = 0.0;
    float pos_b7 = 0.0;
    float pos_b8 = 0.0;

    float pos_w = 1;

    float neg_a0 = 0.0;
    float neg_a1 = 0.0;
    float neg_a2 = 0.0;
    float neg_a3 = 0.0;
    float neg_a4 = 0.0;
    float neg_a5 = 0.0;
    float neg_a6 = 0.0;
    float neg_a7 = 0.0;
    float neg_a8 = 0.0;

    float neg_b1 = 0.0;
    float neg_b2 = 0.0;
    float neg_b3 = 0.0;
    float neg_b4 = 0.0;
    float neg_b5 = 0.0;
    float neg_b6 = 0.0;
    float neg_b7 = 0.0;
    float neg_b8 = 0.0;

    float neg_w = 1;

    float compensation_active = false;

    float theta = 0.0f;
    float dtheta = 0.0f;

    float err = 0.0;
    float derr = 0.0;
    float ierr = 0.0;

    float errPast = 0.0;

    float out;

    utility::AnalogFilter* lowPass;
};

inline ControllerFactory::Builder make_Speed_PI_builder() {
    auto fn = [](std::vector<float> params) -> Controller * {
        if (params.size() < 2)
        return nullptr;

        return new SpeedPI(params[0], params[1], params[2], params[3],
            params[4], params[5], params[6], params[7], params[8],
            params[9], params[10], params[11], params[12], params[13],
            params[14], params[15], params[16], params[17], params[18],
            params[19], params[20], params[21], params[22], params[23],
            params[24], params[25], params[26], params[27], params[28],
            params[29], params[30], params[31], params[32], params[33],
            params[34], params[35], params[36], params[37], params[38]);
    };

    // the empty string log name will be consider as a non valid log
    return {fn, {"Kp", "Ki", "p_a0", "p_a1", "p_a2", "p_a3",
        "p_a4", "p_a5", "p_a6", "p_a7", "p_a8", "p_b1", "p_b2",
        "p_b3", "p_b4", "p_b5", "p_b6", "p_b7", "p_b8", "p_w",
        "n_a0", "n_a1", "n_a2", "n_a3", "n_a4", "n_a5", "n_a6",
        "n_a7", "n_a8", "n_b1", "n_b2", "n_b3", "n_b4", "n_b5",
        "n_b6", "n_b7", "n_b8", "n_w", "comp_enable"},
        {"err", "derr", "ierr"}};
}

}  // namespace forecast

#endif  // SPEED_PI_H