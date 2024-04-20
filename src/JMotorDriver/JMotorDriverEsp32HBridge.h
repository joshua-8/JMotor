#ifndef J_MOTOR_DRIVER_ESP32_HBRIDGE_H
#define J_MOTOR_DRIVER_ESP32_HBRIDGE_H
#include "JMotorDriver.h"
#include <Arduino.h>
/**
 * @brief Controls an H-bridge motor driver with two pins
 * When wiring to an L293D or similar motor driver, connect the enable pin to 5 volts, and the two other input pins to the ESP32.
 * Unlike JMotorDriverEsp32L293, this driver uses two pins to control the motor.
 * Unlike JMotorDriverEsp32HBridgeTwoLedcChannels, this driver uses a single LEDc channel, so this is better than JMotorDriverEsp32HBridgeTwoLedcChannels.
 */
class JMotorDriverEsp32HBridge : public JMotorDriver {
protected:
    bool enabled = false;
    bool reverse;
    bool invertSignals;

    int PWM_FREQ;
    int PWM_RES;
    int PWM_RANGE;

    byte pinPos;
    byte pinNeg;
    byte ch;

public:
    /**
     * @brief Controls an H-bridge motor driver with two pins
     * When wiring to an L293D or similar motor driver, connect the enable pin to 5 volts, and the two other input pins to the ESP32.
     * Unlike JMotorDriverEsp32L293, this driver uses two pins to control the motor.
     * Unlike JMotorDriverEsp32HBridgeTwoChannels, this driver uses a single LEDc channel, so this is better than JMotorDriverEsp32HBridge.
     * @param  _ledCChannel: ledc channel (must be unique for each driver)
     * @param  _pinPos: positive direction pin of motor driver
     * @param  _pinNeg: negative direction pin of motor driver
     * @param  _reverse: invert values (default false)
     * @param  _freq: Hz (default 2000) must be <= int(80E6 / 2^resBits)
     * @param  _resolution: bits of resolution, tradeoff with frequency, default 12
     * @param  _invertSignals: invert signals on both pinPos and pinNeg (default false)
     */
    JMotorDriverEsp32HBridge(byte _ledCChannel, byte _pinPos, byte _pinNeg, bool _reverse = false, int _freq = 2000, int _resolution = 12, bool _invertSignals = false)
    {
        pinPos = _pinPos;
        pinNeg = _pinNeg;
        PWM_FREQ = _freq;
        PWM_RES = _resolution;
        ch = _ledCChannel;
        enabled = false;
        reverse = _reverse;
        invertSignals = _invertSignals;
        PWM_RANGE = (1 << PWM_RES) - 1;
    }
    bool set(float val)
    {
        if (reverse) {
            val = -val;
        }
        if (enabled) {
            val = val * PWM_RANGE;
            val = constrain(val, -PWM_RANGE, PWM_RANGE);
            if (val == 0) {
                ledcDetachPin(pinPos);
                ledcDetachPin(pinNeg);
                write(pinPos, LOW);
                write(pinNeg, LOW);
            } else if (val > 0) {
                ledcDetachPin(pinNeg);
                ledcAttachPin(pinPos, ch);
                writePWM(ch, val);
                write(pinNeg, LOW);
            } else {
                ledcDetachPin(pinPos);
                ledcAttachPin(pinNeg, ch);
                writePWM(ch, -val);
                write(pinPos, LOW);
            }
        }
        return abs(val) < 1.0;
    }
    bool setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                // actually enable
                ledcDetachPin(pinPos);
                ledcDetachPin(pinNeg);
                ledcSetup(ch, PWM_FREQ, PWM_RES);
                write(pinPos, LOW);
                write(pinNeg, LOW);
                enabled = true;
                return true;
            }
        } else { // disable
            if (enabled) {
                // actually disable
                enabled = false;
                ledcDetachPin(pinPos);
                ledcDetachPin(pinNeg);
                write(pinPos, LOW);
                write(pinNeg, LOW);
                return true;
            }
        }
        return false;
    }
    /**
     * @brief  set frequency of pwm
     * @note  side effect: this function turns the motor off, so call set again right after this if you want the motor to turn on again.
     * @param  freq: Hz (default 2000) must be <= int(80E6 / 2^resBits)
     * @param  resBits: (default 12) tradeoff with max available frequency
     * @retval none
     */
    void setFrequencyAndResolution(int freq = 2000, int resBits = 12)
    {
        if (freq == PWM_FREQ && resBits == PWM_RES) {
            return; // already set
        }
        PWM_FREQ = freq;
        PWM_RES = resBits;
        PWM_RANGE = (1 << PWM_RES) - 1;

        ledcDetachPin(pinPos);
        ledcDetachPin(pinNeg);
        ledcSetup(ch, PWM_FREQ, PWM_RES);
        digitalWrite(pinPos, LOW);
        digitalWrite(pinNeg, LOW);
        return;
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

protected:
    /**
     * @brief  just digitalWrite, but with invertSignals
     */
    void write(byte pin, bool state)
    {
        if (invertSignals) {
            state = !state;
        }
        digitalWrite(pin, state);
    }
    /**
     * @brief  just ledcWrite, but with invertSignals
     */
    void writePWM(byte ch, int val)
    {
        if (invertSignals) {
            val = PWM_RANGE - val;
        }
        ledcWrite(ch, val);
    }
};
#endif // J_MOTOR_DRIVER_ESP32_HBRIDGE_H
