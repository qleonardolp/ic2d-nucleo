#include <forecast/App.hpp>
#include <forecast/Controller.hpp>
#include <forecast/ReferenceGenerator.hpp>
#include <forecast/com.hpp>
#include <forecast/platforms/workbench/config/timer.h>
#include <forecast/version.hpp>

#include <debug.hpp>

using namespace forecast;

namespace ticker {

// variables and structure for creating the ticker
volatile bool flag;
volatile uint32_t dt_us; // real dt in micro seconds

TIM_HandleTypeDef mTimUserHandle;

static void interrupt_handler() {
  static uint32_t lastCallTime;
  if (__HAL_TIM_GET_FLAG(&mTimUserHandle, TIM_FLAG_UPDATE) == SET) {
    __HAL_TIM_CLEAR_FLAG(&mTimUserHandle, TIM_FLAG_UPDATE);
    flag = true;
    auto time = us_ticker_read();
    dt_us = time - lastCallTime;
    lastCallTime = time;
  }
}

static bool init(float freq) {
  ENABLE_TICKER();

  uint32_t prescaler = 100;
  uint32_t period =
      static_cast<uint32_t>((float)TICKER_CLOCK_FREQ / prescaler / freq);

  while (period > 65536) {
    DEBUG_INFO("period too high try to find a valid prescaler\n");
    if (++prescaler > 65535) {
      DEBUG_INFO("cannot find a valid prescaler for the frequency %f\n", freq);
      return false;
    }
    period = static_cast<uint32_t>((float)TICKER_CLOCK_FREQ / prescaler / freq);
  }

  while (period < 1) {
    DEBUG_INFO("period too low try to find a valid prescaler\n");
    if (--prescaler < 1) {
      DEBUG_INFO("cannot find a valid prescaler for the frequency %f\n", freq);
      return false;
    }
    period = static_cast<uint32_t>((float)TICKER_CLOCK_FREQ / prescaler / freq);
  }

  mTimUserHandle.Instance = TICKER_HW_TIMER;
  mTimUserHandle.Init.Prescaler = prescaler - 1;
  mTimUserHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  mTimUserHandle.Init.Period = period - 1;
  mTimUserHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

  HAL_TIM_Base_Init(&mTimUserHandle);
  HAL_TIM_Base_Start_IT(&mTimUserHandle);

  HAL_NVIC_SetPriority(TICKER_HW_TIMER_IRQn, 2, 0);
  NVIC_SetVector(TICKER_HW_TIMER_IRQn, (uint32_t)interrupt_handler);
  NVIC_EnableIRQ(TICKER_HW_TIMER_IRQn);

  flag = false;

  return true;
}
}; // namespace ticker

// implementation of the App object
App::App(unsigned int txBuffSz, unsigned int rxBuffSz, unsigned int baudrate)
    : hw(nullptr), pc(USBTX, USBRX, C_8bits, C_Use_RX_TX, baudrate,
                      C_Parity_None, C_1stop, C_RX_TX_DMA, txBuffSz, rxBuffSz),
      txBuffSz(txBuffSz), rxBuffSz(rxBuffSz) {

  DEBUG_INFO("Trying to enable the communication:\n");
  // enabling the communication
  auto usartStatus = pc.enable();
  if (usartStatus != USART_status::S_ok) {
    error.set("Unable to initialize the communication, usart error code: " +
              std::to_string(static_cast<unsigned int>(usartStatus)));
    DEBUG_INFO("%s\n", error.msg().c_str());
    return;
  }

  DEBUG_INFO("\tpc enabled\n");

  usartStatus = pc.transmit_enable();
  if (usartStatus != USART_status::S_ok) {
    error.set("cannot enable the transimt functions. usart error code: " +
              std::to_string(static_cast<unsigned int>(usartStatus)));
    DEBUG_INFO("%s\n", error.msg().c_str());
    return;
  }
  DEBUG_INFO("\ttransmit enabled\n");

  usartStatus = pc.receive_enable();
  if (usartStatus != USART_status::S_ok) {
    error.set("cannot enable the receive functions. usart error code: " +
              std::to_string(static_cast<unsigned int>(usartStatus)));
    DEBUG_INFO("%s\n", error.msg().c_str());
    return;
  }

  DEBUG_INFO("\treceive enabled\n");
}

App::~App() {
  for (size_t i = 0; i < controllers.size(); ++i)
    delete controllers[i];
}

void App::set_hw(IHardware *hw) {
  for (size_t i = 0; i < controllers.size(); ++i)
    delete controllers[i]; // to avoid memory leaks

  if (hw != nullptr) {
    controllers.resize(hw->get_motor_num());

    for (size_t i = 0; i < controllers.size(); ++i)
      controllers[i] = new ControllerWrapper(); // new controllers
  }

  this->hw = hw;
}

uint32_t App::get_ticker_dt() const { return ticker::dt_us; }

void App::send_error(const std::string &msg) {
  send(com::Type::ERROR, msg.c_str(), msg.size() * sizeof(char) + 1);
}

bool App::is_ok() { return not error; }

void App::handle_req_hw_data() {
  DEBUG_INFO("REQ_HW_DATA\n");
  auto logs = hw->logs_names();
  size_t sz = 2; // id and number of motors bytes
  for (auto l : logs)
    sz += l.length() + 1; // add the length of the string

  uint8_t *payload = new uint8_t[sz];
  // adding the id of the packet
  payload[0] = com::Type::REQ_HW_DATA;
  // adding number of motors
  payload[1] = static_cast<uint8_t>(hw->get_motor_num());

  // adding log names
  std::string logs_str = "";
  const int logs_size = logs.size();
  for (int i = 0; i < logs_size; i++) {
    logs_str += logs[i];
    if (i != logs_size - 1) {
      logs_str.append(",");
    }
  }

  // sending with wait
  memcpy(&payload[2], logs_str.c_str(), strlen(logs_str.c_str()));
  // sending the payload
  send(com::Type::ACK, payload, logs_str.size() + 2, true);

  delete[] payload;
}

void App::handle_req_ver(uint8_t *payload) {
  DEBUG_INFO("REQ_VER\n");
  // preparing the payload
  payload[0] = com::Type::REQ_VER;
  memcpy(&payload[1], FORECAST_VERSION, strlen(FORECAST_VERSION));
  // sending the payload
  send(com::Type::ACK, payload, strlen(FORECAST_VERSION) + 1, true);
}

void App::handle_req_ctrl_list(uint8_t *payload) {
  DEBUG_INFO("REQ_CTRL_LIST\n");
  std::string payload_str = "";
  const int sz = controller_factory.get_registered_names().size();
  for (int i = 0; i < sz; i++) {
    payload_str += controller_factory.get_registered_names()[i];
    if (i != sz - 1) {
      payload_str.append(",");
    }
  }

  // preparing the payload
  payload[0] = com::Type::REQ_CTRL_LIST;
  memcpy(&payload[1], payload_str.c_str(), strlen(payload_str.c_str()));
  // sending the payload
  send(com::Type::ACK, payload, payload_str.size() + 1, true);
}

void App::handle_req_ref_gen_list(uint8_t *payload) {
  DEBUG_INFO("REQ_REF_GEN_LIST\n");
  std::string payload_str = "";
  const int sz = ref_gen_factory.get_registered_names().size();
  for (int i = 0; i < sz; i++) {
    payload_str += ref_gen_factory.get_registered_names()[i];
    if (i != sz - 1) {
      payload_str.append(",");
    }
  }

  // preparing the payload
  payload[0] = com::Type::REQ_REF_GEN_LIST;
  memcpy(&payload[1], payload_str.c_str(), strlen(payload_str.c_str()));
  // sending the payload
  send(com::Type::ACK, payload, payload_str.size() + 1, true);
}

void App::handle_req_ctrl_params(uint8_t *payload) {
  DEBUG_INFO("REQ_CTRL_PARAMS\n");
  std::string payload_str = "";
  std::string ctrl_name = reinterpret_cast<const char *>(payload);

  payload_str += ctrl_name;
  payload_str.append(",");

  const size_t sz = controller_factory.get_params(ctrl_name).size();
  for (size_t i = 0; i < sz; i++) {
    auto param = controller_factory.get_params(ctrl_name)[i];
    if (param.empty()) {
      continue;
    }

    payload_str += param;
    if (i != sz - 1) {
      payload_str.append(",");
    }
  }

  DEBUG_INFO("PARAM LIST: %s\n", payload_str.c_str());

  // preparing the payload
  payload[0] = com::Type::REQ_CTRL_PARAMS;
  memcpy(&payload[1], payload_str.c_str(), strlen(payload_str.c_str()));
  // sending the payload
  send(com::Type::ACK, payload, payload_str.size() + 1, true);
}

void App::handle_req_ref_gen_params(uint8_t *payload) {
  DEBUG_INFO("REQ_REF_GEN_PARAMS\n");
  std::string payload_str = "";
  std::string ref_name = reinterpret_cast<const char *>(payload);

  payload_str += ref_name;
  payload_str.append(",");

  const int sz = ref_gen_factory.get_params(ref_name).size();
  for (int i = 0; i < sz; i++) {
    payload_str += ref_gen_factory.get_params(ref_name)[i];
    if (i != sz - 1) {
      payload_str.append(",");
    }
  }

  // preparing the payload
  payload[0] = com::Type::REQ_REF_GEN_PARAMS;
  memcpy(&payload[1], payload_str.c_str(), strlen(payload_str.c_str()));
  // sending the payload
  send(com::Type::ACK, payload, payload_str.size() + 1, true);
}

void App::handle_req_ctrl_logs(uint8_t *payload) {
  DEBUG_INFO("REQ_CTRL_LOGS\n");
  std::string payload_str = "";
  std::string ctrl_name = reinterpret_cast<const char *>(payload);

  payload_str += ctrl_name;
  payload_str.append(",");

  vector<string> logs = controller_factory.get_logs(ctrl_name);

  const int sz = logs.size();
  for (int i = 0; i < sz; i++) {

    if (logs[i].empty())
      continue;

    payload_str += logs[i];
    if (i != sz - 1) {
      payload_str.append(",");
    }
  }

  DEBUG_INFO("LOG LIST: %s\n", payload_str.c_str());
  // preparing the payload
  payload[0] = com::Type::REQ_CTRL_LOGS;
  memcpy(&payload[1], payload_str.c_str(), strlen(payload_str.c_str()));
  // sending the payload
  send(com::Type::ACK, payload, payload_str.size() + 1, true);
}

void App::handle_req_ready(uint8_t *payload) {
  DEBUG_INFO("REQ_READY\n");
  bool ready = true;
  for (const auto &wrapper : controllers) {
    ready = static_cast<bool>(wrapper->ctrl) and
            static_cast<bool>(wrapper->ref_gen);
    if (ready)
      break;
  }

  // preparing the payload
  payload[0] = com::Type::REQ_READY;
  payload[1] = (ready == true ? 1 : 0);
  // sending the payload
  send(com::Type::ACK, payload, 2, true);
}

void App::handle_cmd_set_ctrl(uint8_t *payload, ssize_t size) {
  DEBUG_INFO("CMD_SET_CTRL\n");
  // should be nullterminated
  auto *ctrl_name = reinterpret_cast<const char *>(&payload[0]);

  uint8_t motor = payload[strlen(ctrl_name) + 1 /*\0*/];

  auto *params_ptr =
      reinterpret_cast<float *>(&payload[strlen(ctrl_name) + 2 /*\0*/]);

  auto p_num = (size - strlen(ctrl_name) - 2) / sizeof(float);

  std::vector<float> params(params_ptr, params_ptr + p_num);

  DEBUG_INFO("ctrl name: %s\n", ctrl_name);
  DEBUG_INFO("motor: %d\n", motor);
  for (auto f : params) {
    DEBUG_INFO("%f\n", f);
  }

  if (motor > hw->get_motor_num()) {
    std::string error_msg = "invalid motor index";
    DEBUG_INFO("Error: %s\n", error_msg.c_str());
    payload[0] = com::Type::CMD_SET_CTRL;
    memcpy(&payload[1], error_msg.c_str(), strlen(error_msg.c_str()));
    send(com::NACK, payload, error_msg.size() + 1, true);
    return;
  }

  if (not controller_factory.is_registered(ctrl_name)) {
    std::string error_msg = "unregistered controller";
    DEBUG_INFO("Error: %s\n", error_msg.c_str());
    payload[0] = com::Type::CMD_SET_CTRL;
    memcpy(&payload[1], error_msg.c_str(), strlen(error_msg.c_str()));
    send(com::NACK, payload, error_msg.size() + 1, true);
    return;
  }

  auto *ctrl = controller_factory.build(ctrl_name, params);
  if (ctrl == nullptr) {
    std::string error_msg = "cannot build the controller with "
                            "the given parameters";
    DEBUG_INFO("Error: %s\n", error_msg.c_str());
    payload[0] = com::Type::CMD_SET_CTRL;
    memcpy(&payload[1], error_msg.c_str(), strlen(error_msg.c_str()));
    send(com::NACK, payload, error_msg.size() + 1, true);
    return;
  }

  DEBUG_INFO("control build correctly\n");
  controllers[motor]->ctrl_name = ctrl_name;
  DEBUG_INFO("control build correctly and name set\n");
  controllers[motor]->ctrl.reset(ctrl);
  DEBUG_INFO("CTRL set correctly!\n");
  // preparing the payload
  payload[0] = com::Type::CMD_SET_CTRL;
  // sending the payload
  send(com::Type::ACK, payload, 1, true);
}

void App::handle_cmd_set_hw_logs(uint8_t *payload) {
  DEBUG_INFO("CMD_SET_HW_LOGS\n");
  auto *payload_str = reinterpret_cast<const char *>(&payload[0]);
  DEBUG_INFO("payload: '%s'\n", payload_str);
  std::vector<std::string> logs;

  std::string pp = payload_str;
  size_t l_len = 0, i_start = 0;
  for (size_t i = 0; i < strlen(payload_str); i++) {
    if (payload_str[i] == ',') {
      logs.emplace_back(pp.substr(i_start, l_len));
      l_len = 0;
      i_start = i + 1;
    } else {
      l_len++;
    }
  }

  if (l_len != 0) {
    logs.emplace_back(pp.substr(i_start, l_len));
  }

  hw_logs.clear(); // clearing the log array
  for (auto l : logs) {
    DEBUG_INFO("%s\n", l.c_str());
    hw_logs.emplace_back(hw->get_log(l));
  }

  // preparing the payload
  payload[0] = com::Type::CMD_SET_HW_LOGS;
  // sending the payload
  send(com::Type::ACK, payload, 1, true);
}

void App::handle_cmd_set_ctrl_logs(uint8_t *payload) {
  DEBUG_INFO("CMD_SET_CTRL_LOGS\n");
  uint8_t motor = payload[0];

  auto *payload_str = reinterpret_cast<const char *>(&payload[1]);
  DEBUG_INFO("motor: %d\n", motor);
  DEBUG_INFO("payload: '%s'\n", payload_str);
  std::vector<std::string> logs;

  std::string pp = payload_str;
  size_t l_len = 0, i_start = 0;
  for (size_t i = 0; i < strlen(payload_str); i++) {
    if (payload_str[i] == ',') {
      logs.push_back(pp.substr(i_start, l_len));
      l_len = 0;
      i_start = i + 1;
    } else {
      l_len++;
    }
  }
  if (l_len != 0) {
    logs.push_back(pp.substr(i_start, l_len));
  }

  if (motor > controllers.size()) {
    std::string error_msg = "invalid motor index";
    DEBUG_INFO("Error: %s\n", error_msg.c_str());
    payload[0] = com::Type::CMD_SET_CTRL_LOGS;
    memcpy(&payload[1], error_msg.c_str(), strlen(error_msg.c_str()));
    send(com::NACK, payload, error_msg.size() + 1, true);
    return;
  }

  if (not controller_factory.is_registered(controllers[motor]->ctrl_name)) {
    std::string error_msg = "Logic error in ";
    error_msg.append(__func__);
    error_msg.append(" in file: ");
    error_msg.append(__FILE__);
    error_msg.append(" at line: ");
    error_msg.append(std::to_string(__LINE__ - 7));
    DEBUG_INFO("Error: %s\n", error_msg.c_str());
    payload[0] = com::Type::CMD_SET_CTRL_LOGS;
    memcpy(&payload[1], error_msg.c_str(), strlen(error_msg.c_str()));
    send(com::NACK, payload, error_msg.size() + 1, true);
    return;
  }

  auto available_logs =
      controller_factory.get_logs(controllers[motor]->ctrl_name);

  controllers[motor]->logs.resize(available_logs.size());
  std::fill(controllers[motor]->logs.begin(), controllers[motor]->logs.end(),
            false);

  for (size_t i = 0; i < available_logs.size(); ++i) {
    for (size_t j = 0; j < l_len; ++j)
      if (!logs[j].empty() and logs[j] == available_logs[i]) {
        controllers[motor]->logs[i] = true;
        break;
      }
  }

  for (auto l : logs)
    DEBUG_INFO("%s\n", l.c_str());

  // preparing the payload
  payload[0] = com::Type::CMD_SET_CTRL_LOGS;
  // sending the payload
  send(com::Type::ACK, payload, 1, true);
}

void App::handle_cmd_set_ref_gen(uint8_t *payload, ssize_t size) {
  DEBUG_INFO("CMD_SET_REF_GEN\n");
  // should be nullterminated
  auto *ref_name = reinterpret_cast<const char *>(&payload[0]);

  uint8_t motor = payload[strlen(ref_name) + 1 /*\0*/];

  DEBUG_INFO("MOTOR %u\n", motor);

  auto *params_ptr =
      reinterpret_cast<float *>(&payload[1 + strlen(ref_name) + 1 /*\0*/]);

  auto p_num = (size - strlen(ref_name) - 2) / sizeof(float);

  std::vector<float> params(params_ptr, params_ptr + p_num);

  if (motor > hw->get_motor_num()) {
    auto msg = "invalid motor index";
    send(com::NACK, msg, strlen(msg), true);
    return;
  }

  if (not ref_gen_factory.is_registered(ref_name)) {
    auto msg = "unregistered reference generator";
    send(com::NACK, msg, strlen(msg), true);
    return;
  }

  auto *ctrl = ref_gen_factory.build(ref_name, params);
  if (ctrl == nullptr) {
    auto msg = "cannot build the reference generator with the"
               "given parameters";
    send(com::NACK, msg, strlen(msg), true);
    return;
  }

  controllers[motor]->ref_gen.reset(ctrl);

  // preparing the payload
  payload[0] = com::Type::CMD_SET_REF_GEN;
  // sending the payload
  send(com::Type::ACK, payload, 1, true);
}

void App::handle_cmd_start_loop(uint8_t *payload) {
  DEBUG_INFO("CMD_START_LOOP\n");
  auto *params_ptr = reinterpret_cast<float *>(&payload[0]);

  // 2 float (8 bytes): loop frequency and duration
  auto p_num = 8 / sizeof(float);
  std::vector<float> params(params_ptr, params_ptr + p_num);
  DEBUG_INFO("Loop frequency: %f\n", params[0]);
  DEBUG_INFO("Duration: %f\n", params[1]);

  // preparing the payload
  payload[0] = com::Type::CMD_START_LOOP;
  // sending the payload
  send(com::Type::ACK, payload, 1, true);

  // start the control loop
  exec_control_loop(params[0], params[1]);

  hw->home(); // going home
}

void App::handle_cmd_standby(uint8_t *payload) {
  DEBUG_INFO("CMD_STANDBY\n");
  // preparing the payload
  payload[0] = com::Type::CMD_STANDBY;
  // sending the payload
  send(com::Type::ACK, payload, 1, true);
}

bool App::exec_control_loop(unsigned long freq, float duration) {
  if (not ticker::init(freq)) {
    error.set("cannot intialize the ticker for the desired freq");
    DEBUG_INFO("%s\n", error.msg().c_str());
    return false;
  }

  if (hw == nullptr) {
    error.set("hardware is not been initialized");
    DEBUG_INFO("%s\n", error.msg().c_str());
    return false;
  }

  DEBUG_INFO("Executing the control loop at frequency %luHz\n", freq);

  hw->safety_off(); // putting the hardware in a ready state for the
                    // experiment
  size_t log_size = hw_logs.size();
  for (size_t i = 0; i < controllers.size(); ++i)
    for (size_t j = 0; j < controllers[i]->logs.size(); ++j)
      log_size += controllers[i]->logs[j] ? 1 : 0;

  DEBUG_INFO("LOG SIZE %u\n", log_size);
  float *logs = new float[log_size];
  hw->set_start_time(us_ticker_read() / 1e6);
  while (true) {
    while (not ticker::flag)
      ; // wait for the ticker to be ready
    ticker::flag = false;

    if (pc.is_data_available() == S_data_available) {
      // if the user send data I interrupt the cycle
      // N.B. You can block the cycle with any package, but I'm going to
      //      consume that package expecting it to be a pause command
      com::Type t;
      receive(t, nullptr);

      hw->safety_on(); // hardware in a safety state
                       // (usually not able to run)

      auto byte = com::Type::CMD_STANDBY;
      send(com::Type::ACK, &byte, 1, true); // sending ack to the received mesg

      send_end_log(true, "user interrupt"); // communicating end of logs.
      return true;
    }

    hw->update(ticker::dt_us / 1e6);

    if (hw->get_current_time() > duration) {
      // duration ellapsed
      hw->safety_on();

      send_end_log(true, "timeout");

      return true;
    }

    for (size_t i = 0; i < controllers.size(); ++i) {
      auto &ctrl = controllers[i]->ctrl;
      auto &ref_gen = controllers[i]->ref_gen;

      // check if the controller is set properly
      if (not ctrl) {
        error.set("controller not properly set for motor " + std::to_string(i));
        DEBUG_INFO("%s\n", error.msg().c_str());

        hw->safety_on();
        send_end_log(false, error.msg().c_str());
        return false;
      }
      // check if the reference generator is set properly
      if (not ref_gen) {
        error.set("reference generator not properly set for motor " +
                  std::to_string(i));
        DEBUG_INFO("%s\n", error.msg().c_str());

        hw->safety_on();
        send_end_log(false, error.msg().c_str());
        return false;
      }

      // the controller and ref_gen are available
      auto ref = ref_gen->process(hw);
      float torque = ctrl->process(hw, std::move(ref));

      // setting the torque in the motor
      hw->set_tau_m(i, torque);
    }

    // adding hw logs

    const auto hw_log_size = hw_logs.size();
    for (size_t i = 0; i < hw_log_size; ++i)
      logs[i] = *hw_logs[i];

    // add controllers logs
    for (size_t i = hw_log_size; i < controllers.size() + hw_log_size; ++i) {
      auto ctrl_logs = controllers[i - hw_log_size]->ctrl->get_logs();
      for (size_t j = 0; j < ctrl_logs.size(); ++j)
        if (controllers[i - hw_log_size]->logs[j])
          logs[i] = *ctrl_logs[j];
    }
    // sending the logs

    send(com::Type::LOGS, reinterpret_cast<const void *>(logs),
         log_size * sizeof(float));

    if (ticker::flag) {
      error.set("cannot keep the loop frequency (" + std::to_string(freq) +
                ").Manual reset may be neccessary");
      send_error(error.msg());
      DEBUG_INFO("%s\n", error.msg().c_str());

      hw->safety_on(); // safety

      // A wait is needed to print the error message, otherwise the nucleo
      // just stops the execution
      send_end_log(false, error.msg().c_str());
      return false;
    }
  }
  delete[] logs; // deleting the logs of this loop
}

void App::run() {
  if (error) {
    // not able to run... probably an error occurred in the constructor
    DEBUG_INFO("%s\n", error.msg().c_str());
    fatal_error(error.msg());
  }

  if (hw == nullptr) {
    auto msg = "hardware is not been initialized!\n";
    DEBUG_INFO(msg);
    fatal_error(msg);
  }

  constexpr size_t MAX_PAYLOAD_SIZE = 512;
  uint8_t *payload = new uint8_t[MAX_PAYLOAD_SIZE]; // payload of the messages
  com::Type t;
  ssize_t size;

  DigitalOut led(LED1);

  while (true) {
    size = receive(t, payload, MAX_PAYLOAD_SIZE);

    if (size < 0) {
      send(com::Type::NACK, &size, 1, true);
    }

    DEBUG_INFO("received message: ");
    switch (t) {
    // requests
    case com::Type::REQ_HW_DATA: {
      handle_req_hw_data();
      break;
    }
    case com::Type::REQ_VER: {
      handle_req_ver(payload);
      break;
    }
    case com::Type::REQ_CTRL_LIST: {
      handle_req_ctrl_list(payload);
      break;
    }
    case com::Type::REQ_REF_GEN_LIST: {
      handle_req_ref_gen_list(payload);
      break;
    }
    case com::Type::REQ_CTRL_PARAMS: {
      handle_req_ctrl_params(payload);
      break;
    }
    case com::Type::REQ_REF_GEN_PARAMS: {
      handle_req_ref_gen_params(payload);
      break;
    }
    case com::Type::REQ_CTRL_LOGS: {
      handle_req_ctrl_logs(payload);
      break;
    }
    case com::Type::REQ_READY: {
      handle_req_ready(payload);
      break;
    }

    // commands
    case com::Type::CMD_SET_CTRL: {
      handle_cmd_set_ctrl(payload, size);
      break;
    }
    case com::Type::CMD_SET_HW_LOGS: {
      handle_cmd_set_hw_logs(payload);
      break;
    }
    case com::Type::CMD_SET_CTRL_LOGS: {
      handle_cmd_set_ctrl_logs(payload);
      break;
    }
    case com::Type::CMD_SET_REF_GEN: {
      handle_cmd_set_ref_gen(payload, size);
      break;
    }
    case com::Type::CMD_START_LOOP: {
      handle_cmd_start_loop(payload);
      break;
    }
    case com::Type::CMD_STANDBY: {
      handle_cmd_standby(payload);
      break;
    }

    default: {
      auto err_msg = "unknown package type: " + std::to_string(t);
      DEBUG_INFO("%s\n", err_msg);

      send_error(err_msg);
    }
    }
  }

  delete[] payload;
}

void App::fatal_error(const std::string &msg) {
  send(com::Type::ERROR, msg.c_str(), msg.size() * sizeof(char) + 1);

  wait_ms(1000); // waiting one second

  std::abort();
}

void App::send(com::Type t, const void *data, size_t size, bool wait) {
  auto *pkg = pc.get_tx_buffer_pointer(true);

  auto sz = com::buildPkg(t, data, pkg, size, txBuffSz);

  pc.send(sz, wait); // waiting for the message to be sent
}

ssize_t App::receive(com::Type &t, void *buff, size_t maxSz) {
  // creating the buffer and initializing at 0.
  auto pkg = new uint8_t[rxBuffSz];
  for (size_t i = 0; i < rxBuffSz; ++i)
    pkg[i] = 0u;

  // receiving data
  size_t i = 0;
  DEBUG_INFO("waiting for receive data... ");
  while (not com::checkEtx(pkg, i)) {
    DEBUG_INFO("entering pc.is_data_available");
    while ((pc.is_data_available()) == S_data_unavailable)
      ; // wait
    DEBUG_INFO("exiting pc.is_data_available");

    auto status = pc.get_rx_buffer(&pkg[i]);

    if (status == S_ok or status == S_rx_buffer_empty) {
      DEBUG_INFO("STATUS Ok %d\n", pkg[i]);
      // I've received a byte of data
      i++;
    } else if (status == S_data_unavailable) {
      continue; // Compatibility for using read without dma
    } else if (status == S_rx_buffer_already_empty) {
      DEBUG_INFO("NOT YET DATA\n");
      continue; // Compatitbility for using read with dma
    } else {
      // I'm in a error case something went wrong
      DEBUG_INFO("\n\nERROR aborting i = %d, status = %d\n\n", i, status);
      delete[] pkg; // deleting pkg for avoid memory leaks
      return -1;
    }

    if (i >= rxBuffSz) {
      DEBUG_INFO("\n\nERROR aborting i = %d > rxBuffSz%d\n\n", i, rxBuffSz);
      // return error out of dimension
      delete[] pkg; // deleting pkg for avoid memory leaks
      return -2;
    }
  }

  // checking the pkg, I don't need to check for etx because it's done in the
  // previous while loop
  if (pkg[0] != com::stx) {
    send_error("received a malformed package");
    DEBUG_INFO("Error: no stx\n\tpackage: [");
    size_t i = 0;
    while (not com::checkEtx(pkg, i) and i < rxBuffSz) {
      DEBUG_INFO("%u ", static_cast<unsigned int>(pkg[i++]));
    }
    for (size_t j = 0; j < 3 and i < rxBuffSz; ++j) {
      DEBUG_INFO("%u ", static_cast<unsigned int>(pkg[i++]));
    }
    DEBUG_INFO("\b]\n");
    delete[] pkg; // deleting pkg for avoid memory leaks
    return -3;
  }
  DEBUG_INFO("received!\n");

  t = static_cast<com::Type>(pkg[1]); // returning the type

  // checking for checksum
  auto *escaped = new uint8_t[maxSz + 1];

  ssize_t sz = com::unescape(&pkg[2], escaped, i - 3, maxSz + 1);

  if (escaped[sz - 1] != com::checksum(escaped, sz - 1)) {
    delete[] escaped;
    delete[] pkg;
    return -4;
  }

  if (buff != nullptr)
    memcpy(buff, escaped, sz - 1);

  delete[] escaped; // delete memory leaks

  delete[] pkg; // deleting pkg for avoid memory leaks

  return sz;
}

void App::send_end_log(bool success, const char *msg) {
  size_t sz = strlen(msg) + 1 + 1 /*\0 + success*/;
  auto *payload = new char[sz];

  payload[0] = static_cast<uint8_t>(success);

  if (sz - 2 > 0)
    strncpy(&payload[1], msg, sz - 1);

  send(com::END_LOG, payload, sz, true);

  delete payload;
}

App::ControllerWrapper::ControllerWrapper() : ctrl(nullptr), ref_gen(nullptr) {
  // ntd;
}
