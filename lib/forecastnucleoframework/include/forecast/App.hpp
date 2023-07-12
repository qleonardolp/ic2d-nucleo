#ifndef FORECAST_APP_HPP
#define FORECAST_APP_HPP

#include <USART/USART_interface.hpp>
#include <functional>
#include <memory>
#include <vector>
#include <map>

#include "version.hpp"
#include "Status.hpp"
#include "com.hpp"
#include "debug.hpp"

namespace forecast {

class Controller;  // foreward declaration
class ReferenceGenerator; // foreward declaration
class Operator;
class IHardware;

template <class T>
class Factory {
   public:
    using BuilderFn = std::function<T*(std::vector<float>)>;
    struct Builder {

        Builder(BuilderFn fn, std::vector<std::string> params,
                std::vector<std::string> logs = {}) :
                    builder_fn(std::move(fn)), parameters(std::move(params)),
                    logs(std::move(logs)) {
            // ntd;
        };

        Builder(const Builder &other) : builder_fn(other.builder_fn),
                                   parameters(other.parameters),
                                   logs(other.logs) {
            // ntd;
        }; // copy constructor

        Builder(Builder &&other) : builder_fn(std::move(other.builder_fn)),
                                   parameters(std::move(other.parameters)),
                                   logs(std::move(other.logs)) {
            // ntd;
        }; // move constructor

        BuilderFn builder_fn;
        std::vector<std::string> parameters;
        std::vector<std::string> logs;
    };

    std::map<std::string, Builder> factory;

    bool add(std::string name, BuilderFn fn, std::vector<std::string> params,
             std::vector<std::string> logs = {}) {
         DEBUG_INFO("adding controller %s to the controller factory... ",
                   name.c_str());
        if (factory.find(name) != factory.end()) {
            DEBUG_INFO("Failed\n");
            return false; // key already present
        }

        factory.emplace(name, Builder{std::move(fn), std::move(params),
                                  std::move(logs)});
    }

    bool add(std::string name, Builder builder) {
        DEBUG_INFO("adding controller %s to the controller factory... ",
                   name.c_str());
        if (factory.find(name) != factory.end()) {
            DEBUG_INFO("Failed\n");
            return false; // key already present
        }

        factory.emplace(name, std::move(builder));
        DEBUG_INFO("Done\n");

        return true;
    }

    T *build(std::string name, std::vector<float> params) const {
        if (not is_registered(name))
            return nullptr; // key not present

        return factory.at(name).builder_fn(params);
    }

    inline bool is_registered(const std::string &name) const {
        return not (factory.find(name) == factory.end());
    }

    inline std::vector<std::string> get_params(std::string name) const {
        if (factory.find(name) == factory.end())
            return {};

        return factory.at(name).parameters;
    }

    inline std::vector<std::string> get_logs(std::string name) const {
        if (factory.find(name) == factory.end())
            return {};

        return factory.at(name).logs;
    }

    inline std::vector<std::string> get_registered_names() const {
        std::vector<std::string> ret;
        for(auto const& entry : factory)
            ret.emplace_back(entry.first);

        return ret;
    }
};

using ControllerFactory = Factory<Controller>;
using RefGenFactory = Factory<ReferenceGenerator>;
using OperatorFactory = Factory<Operator>;
class App {
    friend class IHardware;

    struct ControllerWrapper { // TODO move implementation to .cpp
        // default contructor
        ControllerWrapper();
        ~ControllerWrapper() = default;

        // controller
        std::unique_ptr<Controller> ctrl;

        // reference generator for the controller. It's shared for performance
        std::unique_ptr<ReferenceGenerator> ref_gen;

        // in default mod the system won't log anything
        std::vector<bool> logs;

        // necessary to be able to set the log variables
        std::string ctrl_name;
    };

   protected:
    IHardware *hw;      // I don't need to own the object
    USART_interface pc;
    const size_t txBuffSz;
    const size_t rxBuffSz;

    ControllerFactory controller_factory; // used for controller creation
    RefGenFactory ref_gen_factory; // used for reference generators creation
    OperatorFactory operator_factory;

    std::vector<ControllerWrapper *> controllers;
    std::vector<const float *> hw_logs;

    Error error; // used for signaling any possible error

   public:
    App(unsigned int txBuffSz = 256,
        unsigned int rxBuffSz = 256,
        unsigned int baudrate = 921600);

    virtual ~App();

    void set_hw(IHardware *hw);

    /**
     *  @brief Returns a reference to the controller factory
     */
    inline ControllerFactory &get_controller_factory() {
        return controller_factory;
    };

    /**
     * @brief Returns a reference to the reference generator factory
     */
    inline RefGenFactory &get_ref_gen_factory() {
        return ref_gen_factory;
    }

    /**
     * @brief Returns a reference to the reference generator factory
     */
    inline OperatorFactory &get_operator_factory() {
        return operator_factory;
    }

    /**
     * @brief It sends an error through the serial
     *
     */
    void send_error(const std::string& msg);

    /**
     * @brief It sends the required controller logs through the serial
     *
     */
    void send_logs();

    /**
     * @brief Checks if the App is ready to run
     *
     * @return true if the object is fully initialized
     * @return false if the object is not fully initialized
     */
    virtual bool is_ok();

    /**
     * @brief Execs the function execControl at the control frequency.
     *
     * @param[in] freq The frequency of the loop in Hz. Default to 1 KHz
     */
    bool exec_control_loop(unsigned long freq = 1000, float duration = 30);

    /**
     * @brief Execute the main application loop
     *
     * In this loop the application will wait for a command or a request from
     * the pc ("master") and will execute the required action.
     */
    void run();

   protected:
    void send(com::Type t, const void* data, size_t sz, bool wait = false);
    ssize_t receive(com::Type& t, void* buff, size_t maxSz = 0u);


    void send_end_log(bool success, const char *msg);
    /**
     * @brief A fatal error occurred and there is no way to recover
     *
     * In a case like this the application will try to log on the logError and
     * then it will call std::abort().
     *
     * This kind of fatal error could occurre for example in case the app is
     * calling an hardware functionality that is not available.
     *
     * @param msg[in] the message to write on the serial
     */
    void fatal_error(const std::string& msg);

    /**
     * @brief Returns the dt of the ticker that's maintaining the loop
     *
     * N.B. This function should not be called outside the execControl method
     *
     * @return uint32_t
     */
    uint32_t get_ticker_dt() const;

    // Requests
    void handle_req_hw_data();
    void handle_req_ver(uint8_t *payload);
    void handle_req_ctrl_list(uint8_t *payload);
    void handle_req_ref_gen_list(uint8_t *payload);
    void handle_req_ctrl_params(uint8_t *payload);
    void handle_req_ref_gen_params(uint8_t *payload);
    void handle_req_ctrl_logs(uint8_t *payload);
    void handle_req_ready(uint8_t *payload);

    // Commands
    void handle_cmd_set_ctrl(uint8_t *payload, ssize_t size);
    void handle_cmd_set_hw_logs(uint8_t *payload);
    void handle_cmd_set_ctrl_logs(uint8_t *payload);
    void handle_cmd_set_ref_gen(uint8_t *payload, ssize_t size);
    void handle_cmd_start_loop(uint8_t *payload);
    void handle_cmd_standby(uint8_t *payload);
};
}  // namespace forecast

#endif  // FORECAST_APP_HPP