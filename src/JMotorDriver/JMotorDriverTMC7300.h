#ifndef JMOTOR_DRIVER_TMC7300_H
#define JMOTOR_DRIVER_TMC7300_H
#include "JMotorDriver.h"
#include <Arduino.h>
class JMotorDriverTMC7300 : public JMotorDriver { // TODO:
protected:
public:
    JMotorDriverTMC7300()
    {
    }
    bool set(float val)
    {
        return false;
    }
    bool setEnable(bool _enable)
    {
        return false;
    }
    bool getEnable()
    {
        return false;
    }
    float getMaxRange()
    {
        return 1;
    }
    float getMinRange()
    {
        return -1;
    }
    bool enable()
    {
        return false;
    }
    bool disable()
    {
        return false;
    }
};
#endif // JMOTOR_DRIVER_TMC7300_H
