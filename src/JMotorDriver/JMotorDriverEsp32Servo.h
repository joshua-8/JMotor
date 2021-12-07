#ifndef J_MOTOR_DRIVER_ESP32_SERVO_H
#define J_MOTOR_DRIVER_ESP32_SERVO_H
#include "JMotorDriverServoAdvanced.h"
#include <Arduino.h>

/**
 * @brief For servos and motor controllers that use servo signals (ESCs)
 * @note  platform: ESP32
 */
class JMotorDriverEsp32Servo : public JMotorDriverServoAdvanced {
private:
    bool enabled = false;
    byte servoPin;
    byte pwmChannel;
    float SERVO_TICKS_PER_MICROSECOND = 52.4288;
    int SERVO_FREQ = 50;
    int SERVO_RES = 20;

public:
    /**
     * @brief  constructor, sets pins, custom frequency and resolution optional
     * @param  _pwmChannel: ledc channel (must be unique for each driver)
     * @param  _servoPin: pin to output signal on
     * @param  _freq = 50: Hz (default 50) must be <= int(80E6 / 2^resBits)
     * @param  _resBits = 20: (default 20) tradeoff with max available frequency
     * @param  _minServoValue: (int) minimum servo pulse, default: 544 microseconds
     * @param  _maxServoValue: (int) maximum servo pulse, default: 2400 microseconds
     * @param  _constrainRange: (bool) constrain range of set() to within -1 and 1, default: true
     */
    JMotorDriverEsp32Servo(byte _pwmChannel, byte _servoPin, int _freq = 50, int _resBits = 20, int _minServoValue = 544, int _maxServoValue = 2400, bool _constrainRange = true)
    {
        minServoValue = _minServoValue;
        maxServoValue = _maxServoValue;
        enabled = false;
        servoPin = _servoPin;
        pwmChannel = _pwmChannel;
        constrainRange = _constrainRange;
        setFrequencyAndResolution(_freq, _resBits);
    }

    void adjustFrequency(float freq = 1.0)
    {
        setFrequencyAndResolution(freq * 50.0);
    }

    /**
     * @brief  set frequency that servo signal pulse is repeated at and how many bits are used internally for resolution
     * @param  freq: Hz (default 50) must be <= int(80E6 / 2^resBits)
     * @param  resBits: (default 20) tradeoff with max available frequency
     * @retval (float) returns PWM cycles per microsecond-used in ledcWrite call, returned for debugging purposes
     */
    float setFrequencyAndResolution(int freq = 50, int resBits = 20)
    {
        if (freq == SERVO_FREQ && resBits == SERVO_RES) {
            return SERVO_TICKS_PER_MICROSECOND; //already set
        }
        SERVO_FREQ = freq;
        SERVO_RES = resBits;
        SERVO_TICKS_PER_MICROSECOND = (1 << SERVO_RES) * (float)SERVO_FREQ / 1000000; //DEFAULT=52.4288  2^SERVO_RES / 1E6 * SERVO_FREQ
        unsigned long startMicros = micros();
        while (digitalRead(servoPin) == HIGH && micros() - startMicros <= maxServoValue)
            ; //wait for pulse to go low to avoid cutting it short and causing the servo to twitch
        ledcDetachPin(servoPin);
        ledcSetup(pwmChannel, SERVO_FREQ, SERVO_RES);
        if (enabled)
            ledcAttachPin(servoPin, pwmChannel);
        return SERVO_TICKS_PER_MICROSECOND;
    }
    bool set(float _val)
    {
        if (enabled) {
            float val;
            if (constrainRange)
                val = constrain(_val, -1.0, 1.0);
            else
                val = _val;
            setMicroseconds = ((maxServoValue + minServoValue) / 2 + (maxServoValue - minServoValue) * val / 2);
            ledcWrite(pwmChannel, SERVO_TICKS_PER_MICROSECOND * setMicroseconds);
        }
        return abs(_val) < 1.0;
    }
    bool setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                pinMode(servoPin, OUTPUT);
                ledcSetup(pwmChannel, SERVO_FREQ, SERVO_RES);
                ledcAttachPin(servoPin, pwmChannel);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                unsigned long startMicros = micros();
                while (digitalRead(servoPin) == HIGH && micros() - startMicros <= maxServoValue)
                    ; //wait for pulse to go low to avoid cutting it short and causing the servo to twitch
                ledcDetachPin(servoPin);
                pinMode(servoPin, OUTPUT);
                digitalWrite(servoPin, LOW);
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