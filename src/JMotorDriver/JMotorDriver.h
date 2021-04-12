#ifndef J_MOTOR_DRIVER_H
#define J_MOTOR_DRIVER_H
#include <Arduino.h>

class JMotorDriver {

public:
    enum JMotorDriverType {
        Null,
        avrServo,
        espServo,
        avrPWM,
        espPWM,
        avrPWMDir,
        espPWMDir,
        avrL293d,
        espL293d,
    };

    virtual JMotorDriverType getType();
    virtual boolean set(float val);
    virtual boolean setEnable(boolean val);
    virtual boolean getEnable();
    virtual float getMaxRange();
    virtual float getMinRange();
};

#endif