#ifndef J_MOTOR_DRIVER_AVR_SERVO_H
#define J_MOTOR_DRIVER_AVR_SERVO_H
#include "JMotorDriverServo.h"
#include <Arduino.h>
#include <Servo.h>
/**
 * @brief  wraps standard servo library. For servos and motor controllers that use servo signals (ESCs)
 * @note   compatible with all boards that support analogWrite()
 */
class JMotorDriverAvrServo : public JMotorDriverServo {
private:
    bool enabled = false;
    byte servoPin;
    Servo motorServo;

public:
    /**
     * @brief  constructor, sets pins
     * @param  _servoPin: pin to attach servo to
     * @param  _minServoValue: (int) minimum servo pulse, default: 544 microseconds
     * @param  _maxServoValue: (int) maximum servo pulse, default: 2400 microseconds
     */
    JMotorDriverAvrServo(byte _servoPin, int _minServoValue = 544, int _maxServoValue = 2400)
    {
        minServoValue = _minServoValue;
        maxServoValue = _maxServoValue;
        enabled = false;
        servoPin = _servoPin;
    }
    bool set(float _val)
    {
        float val = constrain(_val, -1.0, 1.0);
        if (enabled) {
            if (!motorServo.attached()) {
                motorServo.attach(servoPin);
            }
            motorServo.writeMicroseconds((maxServoValue + minServoValue) / 2 + (maxServoValue - minServoValue) * val / 2);
        }
        return abs(_val) < 1.0;
    }
    bool setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                motorServo.detach();
                enabled = false;
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