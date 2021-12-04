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
     * @param  _timeout: (unsigned long) default: 0, set output to zero if there's been no movement for this many milliseconds. 0=never timeout
     */
    JControlLoopBasic(float kP, unsigned long _timeout = 0)
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
        result = 0;
        if (timeout == 0 || millis() - lastMovedMillis <= timeout) {
            error = (controller->getPosSetpoint() - controller->getPos());
            ctrlLoopOut = P * error;
            result = ctrlLoopOut + controller->getPosDeltaSetpoint();
        }

        return result;
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
