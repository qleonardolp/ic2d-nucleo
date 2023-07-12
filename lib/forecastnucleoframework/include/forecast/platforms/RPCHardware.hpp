#ifndef FORECAST_RPC_HARDWARE_HPP
#define FORECAST_RPC_HARDWARE_HPP

#ifdef TARGET_STM32L4
#include <memory>

#include <hw/DigitalEncoder/DigitalEncoderAB.h>
#include <hw/Motor/EsconMotor.h>
#include <hw/Periphericals/AnalogInput.h>

#include "../IHardware.hpp"

namespace forecast {

class RPCHardware : public IHardware {
    float t = 0.0f;
    float dt = 0.0f;

    float tauM = 0.f;
    float dtauM = 0.f;
    float ddtauM = 0.f;
    float prev_tauM = 0.f;
    float prev_dtauM = 0.f;
    
    float thetaM = 0.f;
    float dthetaM = 0.f;
    float ddthetaM = 0.f;
    float prev_thetaM = 0.f;
    float prev_dthetaM = 0.f;

    float tauS = 0.f;
    float dtauS = 0.f;
    float ddtauS = 0.f;
    float prev_tauS = 0.f;
    float prev_dtauS = 0.f;

    float strain_offset = 0.f;

    float voltage_to_kg = 0.272f / 0.134f; // sensor calibration
    float length = 0.33f;

    // strain gauge sensor
    AnalogInput strain_gauge;

    // digital encoder
    DigitalEncoderAB encoder;

    // motor to control
    std::unique_ptr<EsconMotor> motor;

   private:
    bool motorControlInit();

   public:
    RPCHardware(App& app);
    ~RPCHardware() = default;

    Status init();

    virtual void update(float dt) override;

    // setters
    virtual inline void enableControlMotor() override {
        motor->setEnable(true);
    }

    virtual inline void disableControlMotor() override {
        motor->setEnable(false);
    }

    inline void setControlTorque(float torque) override {
        motor->setTorque(torque);
    }

    inline void setVoltageToKgConstant(float voltage_to_kg) {
        this->voltage_to_kg = voltage_to_kg;
    }

    inline void setSensorLength(float length) {
        this->length = length;
    }

    // getters
    virtual inline float getT() const override { return t; }
    virtual inline float getDT() const override { return dt; }

    virtual inline float getTauM() const override { return tauM; }
    virtual inline float getDTauM() const override { return dtauM; }
    virtual inline float getDDTauM() const override { return ddtauM; }

    virtual inline float getThetaM() const override { return thetaM; }
    virtual inline float getDThetaM() const override { return dthetaM; }
    virtual inline float getDDThetaM() const override { return ddthetaM; }

    virtual inline float getTauS() const override { return tauS; }
    virtual inline float getDTauS() const override { return dtauS; }
    virtual inline float getDDTauS() const override { return ddtauS; }
};

}  // namespace forecast

#endif  // TARGET_STM32L4
#endif  // FORECAST_RPC_HARDWARE_HPP