/**
 * @file Controller.hpp
 * @author AltairLab
 * @brief
 * @version 0.1
 * @date 2023-06-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef FORECAST_CONTROLLER_HPP
#define FORECAST_CONTROLLER_HPP

#include <string>
#include <vector>

#include "IHardware.hpp"

namespace forecast
{  
   /**
    * @brief 
    * 
    */
   class Controller
   {
   protected:
      std::vector<const float *> logs;

   public:
      /**
       * @brief   Controller constructor
       */
      Controller() = default;

      /**
       * @brief Default virtual destructor
       *
       */
      virtual ~Controller() = default;

      /**
       * @brief   Implementation of the control law.
       *          The method produces an output given a reference (input).
       *
       * @param   Control reference
       */
      virtual float process(const IHardware *hw, std::vector<float> ref) = 0;

      /**
       * @brief Get the logs object
       * 
       * @return std::vector<const float *> 
       */
      inline std::vector<const float *> get_logs() const
      {
         // benchmarked: the copy of the vector is faster than the const ref.
         return logs;
      }
   };
} // namespace forecast

#endif