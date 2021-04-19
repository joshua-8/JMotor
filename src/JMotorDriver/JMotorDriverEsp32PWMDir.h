/**
 * @brief  for motor controllers with one direction input and one speed input pin
 * @note   platform: ESP32
 */
#ifndef J_MOTOR_DRIVER_ESP32_PWMDIR_H
#define J_MOTOR_DRIVER_ESP32_PWMDIR_H
#include "JMotorDriver.h"
#include "JMotorDriverEsp32PWM.h"
#include <Arduino.h>
class JMotorDriverEsp32PWMDir : private JMotorDriver {
private:
    boolean enabled = false;
    int dir;

public:
    JMotorDriverEsp32PWM pwmDriver;
    boolean reverse = false;
    /**
     * @brief  constructor, sets pins, default PWM
     * @param  _ch:  ledc channel (must be unique for each driver)
     * @param  _enablePin: pin to output speed signal on
     * @param  _dirPin: pin to output direction signal on
     * @param  _rev = false: if false(default), direction pin set HIGH for positive speed, LOW for negative speed
     */
    JMotorDriverEsp32PWMDir(int _ch, int _enablePin, int _dirPin, boolean _rev = false)
        : pwmDriver { _ch, _enablePin }
    {
        enabled = false;
        dir = _dirPin;
    }
    /**
     * @brief  constructor, sets pins, custom PWM settings
     * @param  _ch: ledc channel (must be unique for each driver)
     * @param  _enPin: pin to output speed signal on
     * @param  _dirPin: pin to output direction signal on
     * @param  freq: <= int(80E6 / 2^resolution), 2kHz default and recommended for motor PWM
     * @param  resolution: bits of resolution, tradeoff with frequency, default 12
     * @param  _rev = false: if false(default), direction pin set HIGH for positive speed, LOW for negative speed
     */
    JMotorDriverEsp32PWMDir(int _ch, int _enPin, int _dirPin, int freq, int resolution, boolean _rev = false)
        : pwmDriver { _ch, _enPin, freq, resolution }
    {
        enabled = false;
        dir = _dirPin;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::esp32PWMDir;
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