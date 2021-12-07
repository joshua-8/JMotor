#ifndef J_MOTOR_DRIVER_ESP32_L293_H
#define J_MOTOR_DRIVER_ESP32_L293_H
#include "JMotorDriver.h"
#include "JMotorDriverEsp32PWM.h"
#include <Arduino.h>

/**
 * @brief  L293 motor driver chip: https://www.ti.com/lit/ds/symlink/l293.pdf
 * also works with L298 drivers or anything with two direction pins and an enable pin!
 * @note   platform: ESP32
 */
class JMotorDriverEsp32L293 : public JMotorDriver {
private:
    bool enabled = false;
    byte i1;
    byte i2;

public:
    JMotorDriverEsp32PWM pwmDriver;
    /**
     * @brief  add extra resistance to motor when set to 0 power (by shorting motor terminals)
     */
    bool breakOn;
    /**
     * @brief  reverse direction of driver
     */
    bool reverse;

    /**
     * @brief  constructor, sets pins and other settings
     * @param  _ch: ledc channel (must be unique for each driver)
     * @param  _enablePin: enable(speed) pin on driver
     * @param  _i1: input pin 1 (direction)
     * @param  _i2: input pin 2 (direction)
     * @param  _breakOn = true: if true (default), add extra resistance to motor when set to 0 power (by shorting motor terminals)
     * @param  _breakWhenDisabled = false: if false (default) turn off break when disabled, if true, keep electrically breaking
     * @param  _reverse (bool) default=false, reverse direction of driver
     * @param  _freq: <= int(80E6 / 2^resolution), 2kHz default and recommended for motor PWM
     * @param  _resolution: bits of resolution, tradeoff with frequency, default 12
     */
    JMotorDriverEsp32L293(byte _ch, byte _enablePin, byte _i1, byte _i2, bool _breakOn = true, bool _breakWhenDisabled = false, bool _reverse = false, int _freq = 2000, int _resolution = 12)
        : pwmDriver { _ch, _enablePin, _freq, _resolution, _breakWhenDisabled }
    {
        enabled = false;
        i1 = _i1;
        i2 = _i2;
        breakOn = _breakOn;
        reverse = _reverse;
    }
    bool set(float val)
    {
        if (reverse) {
            val = -val;
        }
        if (enabled) {
            if (val > 0) {
                digitalWrite(i2, LOW);
                digitalWrite(i1, HIGH);
            } else if (val < 0) {
                digitalWrite(i1, LOW);
                digitalWrite(i2, HIGH);
            } else {
                digitalWrite(i1, LOW);
                digitalWrite(i2, LOW);
            }
            if (breakOn && val == 0) {
                pwmDriver.set(1); //activate break
            } else {
                pwmDriver.set(abs(val));
            }
        }
        return abs(val) < 1.0;
    }
    bool setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, LOW);
                digitalWrite(i2, LOW);
                pwmDriver.setEnable(true);

                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, LOW);
                digitalWrite(i2, LOW);
                pwmDriver.setEnable(false);
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
        return -1.0;
    }
};
#endif
