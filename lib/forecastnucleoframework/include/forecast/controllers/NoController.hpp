#ifndef NO_CONTROLLER_H
#define NO_CONTROLLER_H

#include "../Controller.hpp"

namespace forecast {

/**
 * @brief NoController control class
 **/

class NoController : public Controller {
   public:
   /**
    * @brief No control constructor
    *
    * @param pointer to the HW interface
    **/
   NoController() = default;

   virtual float process(const IHardware* hw, std::vector<float> ref) override {
      return 0.f;
   };
};

inline ControllerFactory::Builder make_no_controller_builder() {
   auto fn = [](std::vector<float> params) -> Controller * {
      return new NoController();
   };

   return {fn, {}, {}};
}


}  // namespace forecast

#endif  // NO_CONTROLLER_H