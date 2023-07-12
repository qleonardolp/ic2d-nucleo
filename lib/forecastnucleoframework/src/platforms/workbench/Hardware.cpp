#ifdef TARGET_STM32F4

#include <forecast/platforms/workbench/Hardware.hpp>
#include <utility/math.hpp>
// hardware configurations
#include <forecast/platforms/workbench/config/AB_encoder_environment.h>
#include <forecast/platforms/workbench/config/AB_encoder_motor.h>
#include <forecast/platforms/workbench/config/AB_encoder_motor_environment.h>
#include <forecast/platforms/workbench/config/analog_torque_sensor.h>
#include <forecast/platforms/workbench/config/escon_motor.h>
#include <forecast/platforms/workbench/config/escon_motor_env.h>
#include <forecast/platforms/workbench/config/spring.h>

forecast::Status forecast::Hardware::init() {
  wait(2.0);

  if (not motorEncoderInit())
    return Status::MOTOR_ENCODER_INIT_ERR;

  if (not envEncoderInit())
    return Status::ENV_ENCODER_INIT_ERR;

  if (not envMotorEncoderInit())
    return Status::ENV_MOTOR_ENCODER_INIT_ERR;

  if (not motorControlInit())
    return Status::CONTROL_MOTOR_INIT_ERR;
  control_motor->setTorque(0.f);

  if (not motorEnvironmentInit())
    return Status::ENV_MOTOR_INIT_ERR;
  env_motor->setTorque(0.f);

  if (not torqueSensorInit())
    return Status::TORQUE_SENSOR_INIT_ERR;

  return Status::NO_ERROR;
}

bool forecast::Hardware::motorEncoderInit() {
  encoder_motor =
      new DigitalEncoderAB(motorEncoder::CPR, motorEncoder::GEAR_RATIO);

  /* Set the encoder timer */
  encoder_motor->setTIM1(); // read position
  encoder_motor->setTIM3(); // read velocity

  return true;
}

bool forecast::Hardware::envEncoderInit() {
  encoder_env = new DigitalEncoderAB(envEncoder::CPR, envEncoder::GEAR_RATIO);

  /* Set the encoder timer */
  encoder_env->setTIM8(); // read position
  encoder_env->setTIM4(); // read velocity

  return true;
}

bool forecast::Hardware::envMotorEncoderInit() {
  encoder_env_motor =
      new DigitalEncoderAB(envMotorEncoder::CPR, envMotorEncoder::GEAR_RATIO);

  /* Set the encoder timer */
  encoder_env_motor->setTIM2(); // read position

  return true;
}

bool forecast::Hardware::motorControlInit() {
  MotorConfiguration conf;
  conf.enable = motorControl::MOTOR_ENABLE_PIN;
  conf.currFeedback = motorControl::MOTOR_CURRENT_FEEDBACK_PIN;
  conf.analog = motorControl::MOTOR_ANALOG_PIN;
  conf.offset_bias = motorControl::offset_bias;
  conf.amp_scale = motorControl::amp_scale; 

  /* Control motor */
  control_motor = new EsconMotor(conf, motorControl::KT, motorControl::JM, motorControl::MAX_CURR);

  return control_motor != nullptr;
}

bool forecast::Hardware::motorEnvironmentInit() {
  MotorConfiguration conf;
  conf.enable = envMotor::MOTOR_ENABLE_PIN;
  conf.currFeedback = envMotor::MOTOR_CURRENT_FEEDBACK_PIN;
  conf.analog = envMotor::MOTOR_ANALOG_PIN;
  conf.offset_bias = envMotor::offset_bias;
  conf.amp_scale = envMotor::amp_scale; 

  /* Environment motor */
  env_motor = new EsconMotor(conf, envMotor::KT, envMotor::JM, envMotor::MAX_CURR);

  return env_motor != nullptr;
}

bool forecast::Hardware::torqueSensorInit() {
  torque_sensor =
      new AnalogInput(TORQUE_SENSOR_PIN, ADC_PCLK2, ADC_Right, ADC_15s, ADC_12b,
                      ADC_Continuous, ADC_Dma, TORQUE_SENSOR_BUFFER_SIZE);

  /* Enable the ADC - In continous mode the ADC start is done automatically */
  auto enabled = torque_sensor->enable();
  tauSensorOffset = torque_sensor->read_average_float() * 3.3f;

  return enabled == -1 ? false : true;
}

void forecast::Hardware::update(float dt) {
  /* Time update */
  this->dt = dt;
  t = us_ticker_read() / 1e6;
  current_time = get_current_time();

  /* Motor encoder update */
  /* Motor encoder is mounted in the opposite position wrt env encoder */
  thetaM = -encoder_motor->getAngleRad();
  // thetaM += 1;
  // dthetaM = (thetaM - prev_thetaM) / dt;
  dthetaM = -encoder_motor->getVelocityRad(dt);
  ddthetaM = (dthetaM - prev_dthetaM) / dt;
  prev_thetaM = thetaM;
  prev_dthetaM = dthetaM;

  /* Environment encoder update */
  thetaE = encoder_env->getAngleRad();
  // dthetaE = (thetaE - prev_thetaE) / dt;
  dthetaE = encoder_env->getVelocityRad(dt);
  ddthetaE = (dthetaE - prev_dthetaE) / dt;
  prev_thetaE = thetaE;
  prev_dthetaE = dthetaE;

  /* Environment motor encoder update */
  thetaEnvMotor = encoder_env_motor->getAngleRad();
  dthetaEnvMotor = (thetaEnvMotor - prev_thetaEnvMotor) / dt;
  ddthetaEnvMotor = (dthetaEnvMotor - prev_dthetaEnvMotor) / dt;
  prev_thetaEnvMotor = thetaEnvMotor;
  prev_dthetaEnvMotor = dthetaEnvMotor;

  /* Control motor update  (from Escon feedback) */
  tauM = control_motor->getTorqueFeedback();
  dtauM = (tauM - prev_tauM) / dt;
  ddtauM = (dtauM - prev_dtauM) / dt;
  prev_tauM = tauM;
  prev_dtauM = dtauM;

  /* Environment motor update  (from Escon feedback) */
  tauE = env_motor->getTorqueFeedback();
  dtauE = (tauE - prev_tauE) / dt;
  ddtauE = (dtauE - prev_dtauE) / dt;
  prev_tauE = tauE;
  prev_dtauE = dtauE;

  // /* Spring torque update */
  tauS = KS * (thetaM - thetaE);
  dtauS = (tauS - prev_tauS) / dt;
  ddtauS = (dtauS - prev_dtauS) / dt;
  prev_tauS = tauS;
  prev_dtauS = dtauS;

  //tauSensor = -((torque_sensor->read_average_float() * 3.3f) - tauSensorOffset) * 7.6075f * 1.418f;
  tauSensor = torque_sensor->read_average_float() * 3.3f;

  dtauSensor = (tauSensor - prev_tauSensor) / dt;
  ddtauSensor = (dtauSensor - prev_dtauSensor) / dt;
  prev_tauSensor = tauSensor;
  prev_dtauSensor = dtauSensor;
}

void forecast::Hardware::home() 
{
  // resetting the encoders to make the board think it's in the starting
  // position
  encoder_motor->reset(0);
  encoder_env->reset(0);
  encoder_env_motor->reset(0);

  // resetting actuators output:
  control_motor->setTorque(0);
  env_motor->setTorque(0);

  wait_ms(500);
}

#endif // TARGET_STM32F4