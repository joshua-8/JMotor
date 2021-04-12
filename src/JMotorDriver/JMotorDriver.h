#ifndef J_MOTOR_DRIVER_H
#define J_MOTOR_DRIVER_H
#include <Arduino.h>

class JMotorDriver {

public:
    enum JMotorDriverType {
        Null,
        avrServo,
        esp32Servo,
        avrPWM,
        esp32PWM,
        avrPWMDir,
        esp32PWMDir,
        avrL293d,
        esp32L293d,
    };

    virtual JMotorDriverType getType();
    virtual boolean set(float val);
    virtual boolean setEnable(boolean val);
    virtual boolean getEnable();
    virtual float getMaxRange();
    virtual float getMinRange();
};

#endif