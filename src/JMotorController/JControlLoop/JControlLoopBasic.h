#ifndef J_CONTROL_LOOP_BASIC_H
#define J_CONTROL_LOOP_BASIC_H
#include "JControlLoop.h"
/**
 * @brief  simple control loop for use with JMotorControllerClosed, can turn off motor if nothing has moved for a while
 */
class JControlLoopBasic : public JControlLoop {
protected:
    float P;
    float timeout;
    unsigned long lastMovedMillis;

public:
    /**
     * @brief  constructor for control loop for use with JMotorControllerClosed
     * @param  kP: (float) proportional to error
     * @param  _timeout: (unsigned long) set output to zero if there's been no movement for this many milliseconds. 0=never timeout
     */
    JControlLoopBasic(float kP, unsigned long _timeout)
    {
        P = kP;
        timeout = _timeout;
        lastMovedMillis = 0;
    }
    float calc(JMotorControllerClosed* controller)
    {
        if (controller->getPosDeltaSetpoint() != 0 || abs(controller->getPosSetpoint() - controller->getPos()) > controller->getDistFromSetpointLimit() || abs(controller->getVel()) > controller->getMinVel()) {
            lastMovedMillis = millis();
        }
        float ret = 0;

        if (timeout == 0 || millis() - lastMovedMillis <= timeout) {
            ret = P * (controller->getPosSetpoint() - controller->getPos()) + controller->getPosDeltaSetpoint();
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
    void resetTime()
    {
        lastMovedMillis = millis();
    }
};
#endif
