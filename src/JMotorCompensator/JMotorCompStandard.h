#ifndef J_MOTOR_COMP_STANDARD_H
#define J_MOTOR_COMP_STANDARD_H
#include "JMotorCompensator.h"
#include "JVoltageCompensator.h"
#include <Arduino.h>
/**
 * @brief  struct for containing settings for JMotorCompStandard
 */
struct JMotorCompStandardConfig {
    /**
     * @brief  if motor settings is within this value from zero, set the motor to zero
     */
    float control_deadzone;
    /**
     * @brief  voltage at which the motor can start turning
     */
    float motor_start_voltage;
    /**
     * @brief  voltage at which the motor stops turning
     */
    float motor_stop_voltage;
    /**
     * @brief  speed the motor turns at motor_start_voltage
     */
    float motor_start_speed;
    /**
     * @brief  defines slope of speed per voltage linear relationship.
     * @note the line extends out from (motor_start_voltage,motor_start_speed)
     */
    float speed_per_volts;
    /**
     * @brief  if motor started at below motor_start_speed, give a boost of motor_start_voltage for start_boost_time milliseconds to override stopping friction
     * @note   set to zero to disable
     */
    unsigned int start_boost_time;

    JMotorCompStandardConfig(float _control_deadzone, float _motor_start_voltage, float _motor_stop_voltage, float _motor_start_speed, float _speed_per_volts, unsigned int _start_boost_time)
    {
        control_deadzone = max(_control_deadzone, 0);
        motor_start_voltage = max(_motor_start_voltage, 0);
        motor_stop_voltage = max(_motor_stop_voltage, 0);
        motor_start_speed = max(_motor_start_speed, 0);
        speed_per_volts = max(_speed_per_volts, 0);
        start_boost_time = _start_boost_time;
    }
};
/**
 * @brief  Converts from speed to driver input compensating for motors not being perfect.
 */
class JMotorCompStandard : public JMotorCompensator {
private:
    JVoltageCompensator& voltComp;
    JMotorCompStandardConfig config;
    boolean startBoosting;
    boolean startBoostingArmed;
    unsigned long startBoostingTimeMillis;

public:
    /**
     * @brief  Converts from speed to driver input compensating for motors not being perfect.
     * @param  _voltComp: reference to JVoltageCompensator to convert voltage to driver input
     * @param  _config: JMotorCompStandardConfig (settings for your motor)
     */
    JMotorCompStandard(JVoltageCompensator& _voltComp, JMotorCompStandardConfig _config)
        : voltComp(_voltComp)
        , config(_config)
    {
        startBoosting = false;
        startBoostingArmed = true;
        startBoostingTimeMillis = 0;
    }
    float compensate(float val)
    {
        float ret = val;
        if (ret == 0) {
            startBoostingArmed = true;
            return 0;
        }
        if (abs(ret) < config.control_deadzone) {
            startBoostingArmed = true;
            return 0;
        }

        if (ret > 0) {
            ret = (1.0 / config.speed_per_volts) * ((ret)-config.motor_start_speed) + config.motor_start_voltage; //point slope form
            ret = max(ret, 0);
        } else {
            ret = (1.0 / config.speed_per_volts) * ((-ret) - config.motor_start_speed) + config.motor_start_voltage; //point slope form
            ret = -max(ret, 0);
        }

        if (!startBoosting && startBoostingArmed && abs(ret) <= config.motor_start_voltage) {
            startBoosting = true;
            startBoostingArmed = false;
            startBoostingTimeMillis = millis();
        }
        Serial.print(ret);
        Serial.print(",");
        if (startBoosting) {
            if (millis() - startBoostingTimeMillis < config.start_boost_time) {
                if (ret > 0) {
                    ret = config.motor_start_voltage;
                } else {
                    ret = -config.motor_start_voltage;
                }
            } else {
                startBoosting = false;
            }
        }

        Serial.println(ret);
        ret = voltComp.adjust(ret);
        return ret;
    }
    void setConfig(JMotorCompStandardConfig& _config)
    {
        config = _config;
    }
};
#endif
