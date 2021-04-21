#ifndef J_MOTOR_DRIVER_AVR_PWMDIR_H
#define J_MOTOR_DRIVER_AVR_PWMDIR_H
#include "JMotorDriver.h"
#include "JMotorDriverAvrPWM.h"
#include <Arduino.h>

/**
 * @brief  for motor controllers with one direction input and one speed input pin
 * @note   platform: AVR (standard Arduinos)
 */
class JMotorDriverAvrPWMDir : private JMotorDriver {
private:
    boolean enabled = false;
    int dir;

public:
    boolean reverse = false;
    JMotorDriverAvrPWM pwmDriver;
    /**
     * @brief  constructor, sets pins
     * @note   speed pin must be PWM capable
     * @param  _speedPin: speed input to motor controller (PWM)
     * @param  _dirPin: direction input to motor controller (digital)
     * @param  _rev = false: if false(default), direction pin set HIGH for positive speed, LOW for negative speed
     */
    JMotorDriverAvrPWMDir(int _speedPin, int _dirPin, boolean _rev = false)
        : pwmDriver { _speedPin }
    {
        enabled = false;
        dir = _dirPin;
        reverse = _rev;
    }
    boolean set(float val)
    {
        if (enabled) {
            if (val > 0) {
                digitalWrite(dir, !reverse);
            } else if (val < 0) {
                digitalWrite(dir, reverse);
            } else {
            }
            pwmDriver.set(abs(val));
        }
        return abs(val) < 1.0;
    }
    boolean setEnable(boolean enable)
    {
        if (enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                pinMode(dir, OUTPUT);
                pwmDriver.setEnable(true);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                pwmDriver.setEnable(false);
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
        return -1.0;
    }
};

#endif