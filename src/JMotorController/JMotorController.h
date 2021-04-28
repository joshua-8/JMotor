#ifndef J_MOTOR_CONTROLLER
#define J_MOTOR_CONTROLLER
#include "JMotorDriver/JMotorDriver.h"
class JMotorController {
protected:
    JMotorDriver& driver;

public:
    virtual void setVel(float vel);
    virtual float getVel(float vel);
    virtual float getDriverSetVal();
    virtual void setPosTarget(float pt);
    virtual float getPos();
    virtual float resetPos(float pos = 0);
    virtual void run();
    virtual boolean setEnable(boolean _enable);
    boolean enable()
    {
        return setEnable(true);
    }
    boolean disable()
    {
        return setEnable(false);
    }
};
#endif
