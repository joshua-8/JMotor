/**
 * @brief  L293 motor driver chip: https://www.ti.com/lit/ds/symlink/l293.pdf
 * @note   platform: ESP32
 */
#ifndef J_MOTOR_DRIVER_ESP32_L293_H
#define J_MOTOR_DRIVER_ESP32_L293_H
#include "JMotorDriver.h"
#include "JMotorDriverEsp32PWM.h"
class JMotorDriverESP32L293 : private JMotorDriver {
private:
    boolean enabled = false;
    int i1;
    int i2;

public:
    JMotorDriverEsp32PWM pwmDriver;
    /**
 * @brief  constructor, sets up pins, default PWM
 * @param  _ch: ledc channel (must be unique for each driver)
 * @param  _enablePin: enable(speed) pin on driver
 * @param  _i1: input pin 1
 * @param  _i2: input pin 2
 */
    JMotorDriverESP32L293(int _ch, int _enablePin, int _i1, int _i2)
        : pwmDriver { _ch, _enablePin }
    {
        enabled = false;
        i1 = _i1;
        i2 = _i2;
    }
    /**
     * @brief  constructor, sets up pins, custom PWM settings
     * @param  _ch: ledc channel (must be unique for each driver)
     * @param  _enPin: enable(speed) pin on driver
     * @param  _i1: input pin 1 (direction)
     * @param  _i2: input pin 2 (direction)
     * @param  freq: <= int(80E6 / 2^resolution), 2kHz default and recommended for motor PWM
     * @param  resolution: bits of resolution, tradeoff with frequency, default 12
     */
    JMotorDriverESP32L293(int _ch, int _enPin, int _i1, int _i2, int freq, int resolution)
        : pwmDriver { _ch, _enPin, freq, resolution }
    {
        enabled = false;
        i1 = _i1;
        i2 = _i2;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::esp32L293;
    }
    boolean set(float val)
    {
        if (enabled) {
            if (val > 0) {
                digitalWrite(i2, LOW);
                digitalWrite(i1, HIGH);
            } else if (val < 0) {
                digitalWrite(i1, LOW);
                digitalWrite(i2, HIGH);
            } else {
                digitalWrite(i1, HIGH);
                digitalWrite(i2, HIGH);
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
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, HIGH);
                digitalWrite(i2, HIGH);
                pwmDriver.setEnable(true);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                pwmDriver.setEnable(false);
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, LOW);
                digitalWrite(i2, LOW);
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