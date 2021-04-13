/**
 * @brief  uses ledc to output PWM approximation of an analog output
 * @note   platform: ESP32
 */
#ifndef J_MOTOR_DRIVER_ESP32_PWM_H
#define J_MOTOR_DRIVER_ESP32_PWM_H
#include "JMotorDriver.h"
class JMotorDriverEsp32PWM : private JMotorDriver {
private:
    int PWM_RES = 12;
    int PWM_FREQ = 2000; // <= int(80E6 / 2^PWM_RES), 2kHz recommended for motor PWM
    int PWM_RANGE = 4095; // 2^PWM_RES -1
    boolean enabled = false;
    int ch;
    int pin;

public:
    boolean disableState = LOW;
    /**
    * @brief  constructor, sets up pins, default PWM
    * @param  _ch: ledc channel (must be unique for each driver)
    * @param  _pin: pin to output signal on
    * @param  _disableState = LOW: when disabled, set pin LOW(default) or HIGH
    */
    JMotorDriverEsp32PWM(int _ch, int _pin, boolean _disableState = LOW)
    {
        enabled = false;
        ch = _ch;
        pin = _pin;
        disableState = _disableState;
    }
    /**
     * @brief  constructor, sets up pins, custom PWM settings
     * @param  _ch:  ledc channel (must be unique for each driver)
     * @param  _pin: pin to output signal on
     * @param  freq: <= int(80E6 / 2^resolution), 2kHz default and recommended for motor PWM
     * @param  resolution: bits of resolution, tradeoff with frequency, default 12
     * @param  _disableState = LOw: when disabled, set pin LOW(default) or HIGH
     */
    JMotorDriverEsp32PWM(int _ch, int _pin, int freq, int resolution, boolean _disableState = LOW)
    {
        enabled = false;
        ch = _ch;
        pin = _pin;
        disableState = _disableState;
        PWM_RES = resolution;
        PWM_FREQ = freq;
        PWM_RANGE = (1 << PWM_RES) - 1;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::esp32PWM;
    }
    boolean set(float _val)
    {
        if (enabled) {
            int val = constrain(_val * PWM_RANGE, 0, PWM_RANGE);
            ledcWrite(ch, val);
        }
        return (_val >= 1.0) || (_val <= 0);
    }
    boolean setEnable(boolean enable)
    {
        if (enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                ledcSetup(ch, PWM_FREQ, PWM_RES);
                ledcAttachPin(pin, ch);
                pinMode(pin, OUTPUT);
                ledcWrite(ch, !disableState ? 0 : PWM_RANGE);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                ledcDetachPin(pin);
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
        return 0;
    }
};

#endif