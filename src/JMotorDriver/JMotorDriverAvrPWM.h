#ifndef J_MOTOR_DRIVER_AVR_PWM_H
#define J_MOTOR_DRIVER_AVR_PWM_H
#include "JMotorDriver.h"
#include <Arduino.h>

/**
 * @brief  wrapper for analogWrite()
 * @note   compatible with all boards that support analogWrite()
 */
class JMotorDriverAvrPWM : public JMotorDriver {
private:
    bool enabled = false;
    byte pin;

public:
    /**
     * @brief  change PWM_RANGE to match the range of analogWrite (change after using analogWriteResolution)
     */
    int PWM_RANGE = 255;
    bool disableState = LOW;
    /**
     * @brief  constructor, sets pins
     * @note   make sure to check which pins on your board are PWM capable
     * @param  _pin: what pin to use
     * @param  _disableState = LOW: when disabled, set pin LOW(default) or HIGH
     */
    JMotorDriverAvrPWM(byte _pin, bool _disableState = LOW)
    {
        enabled = false;
        pin = _pin;
        _disableState = disableState;
    }
    bool set(float _val)
    {
        if (enabled) {
            int val = constrain(_val * PWM_RANGE, 0, PWM_RANGE);
            analogWrite(pin, val);
        }
        return (_val > 0.0) && (_val < 1.0);
    }
    bool setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                analogWrite(pin, !disableState ? 0 : PWM_RANGE);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                analogWrite(pin, !disableState ? 0 : PWM_RANGE);
                return true;
            }
        }
        return false;
    }
    bool getEnable()
    {
        return enabled;
    }
    float getMaxRange()
    {
        return 1.0;
    }
    float getMinRange()
    {
        return 0.0;
    }
};

#endif