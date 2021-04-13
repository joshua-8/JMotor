/**
 * @brief  wrapper for analogWrite()  https://www.ti.com/lit/ds/symlink/l293.pdf
 * @note   platform: AVR (standard Arduinos)
 */
#ifndef J_MOTOR_DRIVER_AVR_PWM_H
#define J_MOTOR_DRIVER_AVR_PWM_H
#include "JMotorDriver.h"
class JMotorDriverAvrPWM : public JMotorDriver {
private:
    boolean enabled = false;
    int pin;

public:
    const int PWM_RANGE = 255;
    boolean disableState = LOW;
    /**
     * @brief  constructor, sets up pins
     * @note   make sure to check which pins on your board are PWM capable
     * @param  _pin: what pin to use
     * @param  _disableState = LOW: when disabled, set pin LOW(default) or HIGH
     */
    JMotorDriverAvrPWM(int _pin, boolean _disableState = LOW)
    {
        enabled = false;
        pin = _pin;
        _disableState = disableState;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::avrPWM;
    }
    boolean set(float _val)
    {
        if (enabled) {
            int val = constrain(_val * PWM_RANGE, 0, PWM_RANGE);
            analogWrite(pin, val);
        }
        return (_val >= 1.0) || (_val <= 0);
    }
    boolean setEnable(boolean enable)
    {
        if (enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                pinMode(pin, OUTPUT);
                analogWrite(pin, !disableState ? 0 : PWM_RANGE);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                pinMode(pin, OUTPUT);
                digitalWrite(pin, disableState);
                return true;
            }
        }
        return false;
    }
    boolean getEnable()
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