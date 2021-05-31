#ifndef J_MOTOR_COMP_BASIC_H
#define J_MOTOR_COMP_BASIC_H
#include "JMotorCompensator.h"
#include "JVoltageCompensator.h"
#include <Arduino.h>
/**
 * @brief  Converts from speed to driver input compensating for variable supply voltage
 */
class JMotorCompBasic : public JMotorCompensator {
private:
    JVoltageCompensator& voltComp;
    float multiplier;
    float minSpeed;

public:
    /**
     * @brief  Converts from speed to driver input compensating for variable supply voltage
     * @param  _voltComp: reference to JVoltageCompensator to convert voltage to driver input
     * @param  _voltsPerSpeed: multiplier for speed input, units: voltage per speed
     * @param  _minSpeed: if the motor is told to go slower than this speed, just turn it off
     */
    JMotorCompBasic(JVoltageCompensator& _voltComp, float _voltsPerSpeed, float _minSpeed)
        : voltComp(_voltComp)
    {
        multiplier = _voltsPerSpeed;
        minSpeed = abs(_minSpeed);
    }
    float compensate(float val)
    {
        if (val == 0 || abs(val) < minSpeed) {
            return 0;
        }
        return constrain(voltComp.adjust(val * multiplier, driverRange), -maxDriverRange, maxDriverRange);
    }
    float getMaxVel()
    {
        return voltComp.getSupplyVoltage() / multiplier;
    }
    float getMinVel()
    {
        return minSpeed;
    }

    void setVoltsPerSpeed(float _voltsPerSpeed)
    {
        multiplier = _voltsPerSpeed;
    }
    float getVoltsPerSpeed()
    {
        return multiplier;
    }
    void setMinSpeed(float _speed)
    {
        minSpeed = abs(_speed);
    }
};
#endif
