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
     * @brief  voltage at which the motor can start turning
     */
    float motor_start_voltage;
    /**
     * @brief  speed the motor turns at motor_start_voltage
     */
    float motor_start_speed;
    /**
     * @brief  voltage at which the motor stops turning
     */
    float motor_stop_voltage;
    /**
     * @brief  speed the motor turns at motor_stop_voltage
     */
    float motor_stop_speed;
    /**
     * @brief  voltage used for calibration point towards fastest speed of motor
     */
    float motor_high_voltage;
    /**
     * @brief  speed the motor turns at motor_high_voltage
     */
    float motor_high_speed;
    /**
     * @brief  how long (in milliseconds) to pulse motor_start_voltage to get the motor started
     */
    unsigned int start_boost_time;

    JMotorCompStandardConfig(
        float _motor_stop_voltage, float _motor_stop_speed,
        float _motor_start_voltage, float _motor_start_speed,
        float _motor_high_voltage, float _motor_high_speed,
        unsigned int _start_boost_time)
    {
        motor_stop_voltage = max(_motor_stop_voltage, (float)0.0);
        motor_stop_speed = max(_motor_stop_speed, (float)0.0);
        motor_start_voltage = max(_motor_start_voltage, (float)0.0);
        motor_start_speed = max(_motor_start_speed, (float)0.0);
        motor_high_voltage = max(_motor_high_voltage, (float)0.0);
        motor_high_speed = max(_motor_high_speed, (float)0.0);
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
    bool startBoosting;
    bool startBoostingArmed;
    unsigned long startBoostingTimeMillis;
    float multiplier;

public:
    /**
     * @brief  Converts from speed to driver input compensating for motors not being perfect.
     * @param  _voltComp: reference to JVoltageCompensator to convert voltage to driver input
     * @param  _config: JMotorCompStandardConfig (settings for your motor)
     * @param  _multiplier: multiplier for speed input, optional, default 1.0
     */
    JMotorCompStandard(JVoltageCompensator& _voltComp, JMotorCompStandardConfig _config, float _multiplier = 1.0)
        : voltComp(_voltComp)
        , config(_config)
    {
        startBoosting = false;
        startBoostingArmed = true;
        startBoostingTimeMillis = 0;
        multiplier = _multiplier;
    }
    float compensate(float val)
    {
        float ret = val * multiplier;
        if (ret == 0) {
            startBoostingArmed = true;
            return 0;
        }

        if (ret > 0) {
            ret = calcSlope(ret);
            ret = max(ret, (float)0);
        } else {
            ret = calcSlope(-ret);
            ret = -max(ret, (float)0);
        }

        if (ret == 0) {

            startBoostingArmed = true;
            return 0;
        }

        if (!startBoosting && startBoostingArmed && abs(ret) <= config.motor_start_voltage) {
            startBoosting = true;
            startBoostingArmed = false;
            startBoostingTimeMillis = millis();
        }
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
        ret = voltComp.adjust(ret, driverRange);
        return constrain(ret, -maxDriverRange, maxDriverRange);
    }
    void setConfig(JMotorCompStandardConfig& _config)
    {
        config = _config;
    }

    float getMaxVel()
    {
        if (voltComp.getSupplyVoltage()*maxDriverRange < config.motor_stop_voltage) {
            return 0;
        } else if (voltComp.getSupplyVoltage() < config.motor_start_voltage) {
            return floatMap(voltComp.getSupplyVoltage(), config.motor_stop_voltage, config.motor_start_voltage, config.motor_stop_speed, config.motor_start_speed) / multiplier;
        } else {
            return floatMap(voltComp.getSupplyVoltage(), config.motor_start_voltage, config.motor_high_voltage, config.motor_start_speed, config.motor_high_speed) / multiplier;
        }
    }

    float getMinVel()
    {
        return config.motor_stop_speed / multiplier;
    }

private:
    float calcSlope(float ret)
    {
        if (ret <= config.motor_stop_speed) {
            ret = 0; //can't turn the motor this slowly
        } else if (ret <= config.motor_start_speed) {
            ret = floatMap(ret, config.motor_stop_speed, config.motor_start_speed, config.motor_stop_voltage, config.motor_start_voltage);
        } else {
            ret = floatMap(ret, config.motor_start_speed, config.motor_high_speed, config.motor_start_voltage, config.motor_high_voltage);
        }
        return ret;
    }
    float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};
#endif
