#ifndef J_CONTROL_LOOP_BASIC_H
#define J_CONTROL_LOOP_BASIC_H
#include "JControlLoop.h"
class JControlLoopBasic : public JControlLoop {
protected:
    float P;
    float timeout;
    unsigned long lastMovedMillis;

public:
    JControlLoopBasic(float kP, unsigned long _timeout)
    {
        P = kP;
        timeout = _timeout;
        lastMovedMillis = 0;
    }
    float calc(JMotorControllerClosed* controller)
    {
        if (controller->getPosDeltaSetpoint() != 0 || abs(controller->getPosTarget() - controller->getPos()) > controller->getDistFromSetpointLimit() || abs(controller->getVel()) > controller->getMinVel()) {
            lastMovedMillis = millis();
        }
        float ret = 0;

        if (timeout == 0 || millis() - lastMovedMillis <= timeout) {
            ret = P * (controller->getPosTarget() - controller->getPos()) + controller->getPosDeltaSetpoint();
        }

        return ret;
    }

    void setKP(float _p)
    {
        P = _p;
    }
    float getKP()
    {
        return P;
    }
    void setTimeout(unsigned long _timeout)
    {
        timeout = _timeout;
    }
    unsigned long getTimeout()
    {
        return timeout;
    }
    void wake()
    {
        lastMovedMillis = millis();
    }
};
#endif
