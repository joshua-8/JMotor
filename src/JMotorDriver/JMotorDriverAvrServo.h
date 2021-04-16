/**
 * @brief  wraps standard servo library. For servos and motor controllers that use servo signals (ESCs)
 * @note   platform: AVR (standard Arduinos)
 */
#ifndef J_MOTOR_DRIVER_AVR_SERVO_H
#define J_MOTOR_DRIVER_AVR_SERVO_H
#include "JMotorDriver.h"
#include <Servo.h>
class JMotorDriverAvrServo : private JMotorDriver {
private:
    boolean enabled = false;
    int servoPin;
    Servo motorServo;

public:
    int minServoValue = 544; // can be changed while running
    int maxServoValue = 2400; // can be changed while running
    /**
     * @brief  constructor, sets pins
     * @param  _servoPin: pin to attach servo to
     */
    JMotorDriverAvrServo(int _servoPin)
    {

        enabled = false;
        servoPin = _servoPin;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::avrServo;
    }
    boolean set(float _val)
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
    boolean setEnable(boolean enable)
    {
        if (enable) {
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