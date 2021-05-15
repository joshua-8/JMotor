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
     * can be changed while running
     */
    int minServoValue = 544;
    /**
     * can be changed while running
     */
    int maxServoValue = 2400;

    /**
     * @brief  constructor, sets pins, custom frequency and resolution optional
     * @param  _pwmChannel: ledc channel (must be unique for each driver)
     * @param  _servoPin: pin to output signal on
     * @param  _freq = 50: Hz (default 50) must be <= int(80E6 / 2^resBits)
     * @param  _resBits = 20: (default 20) tradeoff with max available frequency
     */
    JMotorDriverEsp32Servo(byte _pwmChannel, byte _servoPin, int _freq = 50, int _resBits = 20, int _minServoValue = 544, int _maxServoValue = 2400)
    {
        minServoValue = _minServoValue;
        maxServoValue = _maxServoValue;
        enabled = false;
        servoPin = _servoPin;
        pwmChannel = _pwmChannel;
        setFrequencyAndResolution(_freq, _resBits);
    }

    void adjustFrequency(float freq = 1.0)
    {
        setFrequencyAndResolution(freq * 50);
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
        SERVO_TICKS_PER_MICROSECOND = (1 << SERVO_RES) * SERVO_FREQ / 1000000; //DEFAULT=52.4288  2^SERVO_RES / 1E6 * SERVO_FREQ
        ledcSetup(pwmChannel, SERVO_FREQ, SERVO_RES);
        return SERVO_TICKS_PER_MICROSECOND;
    }
    bool set(float _val)
    {
        if (enabled) {
            ledcWrite(pwmChannel, SERVO_TICKS_PER_MICROSECOND * ((maxServoValue + minServoValue) / 2 + (maxServoValue - minServoValue) * constrain(_val, -1.0, 1.0) / 2));
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
                while (digitalRead(servoPin) == HIGH)
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