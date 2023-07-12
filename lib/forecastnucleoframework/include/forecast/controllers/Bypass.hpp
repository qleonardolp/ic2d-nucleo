#ifndef BYPASS_HPP
#define BYPASS_HPP

#include "../Controller.hpp"

namespace forecast
{
    /**
     * @brief Bypass control class
     **/

    class Bypass : public Controller
    {
    public:
        /**
         * @brief Construct a new object. This constructor initialize,
         * the controller.
         */
        Bypass();

        /**
         * @brief Bypass process function
         * 
         * @param hw        Hardware instance
         * @param ref       Reference value
         * @return float    Output
         */
        virtual float process(const IHardware *hw, std::vector<float> ref) override;

    protected:
        float reference = 0.0; /*! reference var to logging*/
    };

    /**
     * @brief Builder function for the add method of App class
     * 
     * @return ControllerFactory::Builder 
     */
    inline ControllerFactory::Builder make_Bypass_builder()
    {
        auto fn = [](std::vector<float> params) -> Controller *
        {
            return new Bypass();
        };

        return {fn, {}, {"Reference"}};
    }

} // namespace forecast

#endif // BYPASS_H