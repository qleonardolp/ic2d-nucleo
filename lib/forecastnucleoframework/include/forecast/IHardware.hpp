#ifndef FORECAST_IHARDWARE_HPP
#define FORECAST_IHARDWARE_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

#include "App.hpp"

namespace forecast {
class IHardware {
protected:
  App &app;

  std::map<std::string, const float *> logs;

public:
  /**
   * @brief   Hardware constructor
   */
  IHardware(App &app) : app(app) { DEBUG_INFO("IHardware called"); };

  /**
   * @brief Returns the number of motors in the hardware
   */
  virtual size_t get_motor_num() const { return 0; }

  /**
   * @brief Returns the number of position sensors available
   */
  virtual size_t get_theta_sensor_num() const { return 0; }

  /**
   * @brief Returns the number of torque sensors available
   */
  virtual size_t get_tau_sensor_num() const { return 0; }

  inline const float *get_log(const std::string &id) const {
    if (logs.find(id) == logs.end())
      return nullptr; // TODO fix it

    return logs.at(id);
  }

  inline std::vector<std::string> logs_names() const {
    std::vector<std::string> names(logs.size());
    size_t i = 0;
    for (auto const &entry : logs) {
      names[i++] = entry.first;
    }

    return names;
  }

  virtual void safety_on() { /* ntd */
  }

  virtual void safety_off() { /* ntd */
  }

  /**
   * @brief   Return the hw time t of the last update.
   *fix
   * @return  t
   */
  virtual float get_t() const {
    app.fatal_error("Hardware getT() function not implemented");
  }
  /**
   * @brief   Return the hw dt used in the last update.
   *
   * @return  dt
   */
  virtual float get_dt() const {
    app.fatal_error("Hardware getDT() function not implemented");
  }

  /**
   * @brief   Set the start time of the experiment.
   *
   * @param  time The start time of the experiment
   */
  virtual inline void set_start_time(float time) {
    app.fatal_error("Hardware setStartT() function not implemented");
  }

  /**
   * @brief   Set the experiment duration.
   *
   * @param  duration The experiment duration (seconds)
   */
  virtual inline void set_duration(float duration) {
    app.fatal_error("Hardware set_duration() function not implemented");
  }

  /**
   * @brief   Return the start time of the experiment.
   *
   * @return  start_t
   */
  virtual inline float get_start_time() const {
    app.fatal_error("Hardware getStartT() function not implemented");
  }

  /**
   * @brief   Return the hw time t from the start of the experiment.
   *
   * @return  curr_t
   */
  virtual inline float get_current_time() const {
    app.fatal_error("Hardware getStartT() function not implemented");
  }

  /**
   * @brief   Return the hw duration time.
   *
   * @return  duration_t
   */
  virtual inline float get_duration() const {
    app.fatal_error("Hardware get_duration() function not implemented");
  }

  /**
   * @brief   Return the torque applied by the motor (current feedback)
   *
   * @return  motorTorqueFeedback
   */
  virtual inline float get_tau_m(size_t motor_idx) const {
    app.fatal_error("Hardware set_tau_M function not implemented");
  };
  virtual inline float get_d_tau_m(size_t motor_idx) const {
    app.fatal_error("Hardware set_d_tau_M function not implemented");
  };
  virtual inline float get_dd_tau_m(size_t motor_idx) const {
    app.fatal_error("Hardware set_dd_tau_M function not implemented");
  };

  /**
   * @brief   Return the torque measured by the torque sensor
   *
   * @return  tau_s
   */
  virtual inline float get_tau_s(size_t sensor_idx) const {
    app.fatal_error("Hardware getTauS() function not implemented");
  }
  virtual inline float get_d_tau_s(size_t sensor_idx) const {
    app.fatal_error("Hardware getDTauS() function not implemented");
  }
  virtual inline float get_dd_tau_s(size_t sensor_idx) const {
    app.fatal_error("Hardware getDDTauS() function not implemented");
  }

  /**
   * @brief   Return the angle radius measured by the position sensor.
   *
   * @return  thetaM
   */
  virtual inline float get_theta(size_t sensor_idx) const {
    app.fatal_error("Hardware getThetaM() function not implemented");
  }
  virtual inline float get_d_theta(size_t sensor_idx) const {
    app.fatal_error("Hardware getDThetaM() function not implemented");
  }
  virtual inline float get_dd_theta(size_t sensor_idx) const {
    app.fatal_error("Hardware getDDThetaM() function not implemented");
  }

  virtual inline float get_output() const {
    app.fatal_error("Hardware getOutput() function not implemented");
  }
  virtual inline float get_d_output() const {
    app.fatal_error("Hardware getDOutput() function not implemented");
  }
  virtual inline float get_dd_output() const {
    app.fatal_error("Hardware getDDOutput() function not implemented");
  }

  virtual void set_tau_m(size_t motor_idx, float value) {
    app.fatal_error("Hardware set_tau_M function not implemented");
  };

  virtual void update(float dt) = 0;

  virtual void home() = 0; // pure virtual function to go home 
};
} // namespace forecast

#pragma GCC diagnostic pop

#endif // FORECAST_IHARDWARE_HPP