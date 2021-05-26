#ifndef J_CONTROL_LOOP_BASIC_H
#define J_CONTROL_LOOP_BASIC_H
#include "JControlLoop.h"
class JControlLoopBasic : public JControlLoop {
private:
    float P;

public:
    JControlLoopBasic(float kP)
    {
        P = kP;
    }
    float calc(JMotorControllerClosed* controller)
    {
        return P * (controller->getPosSetpoint() - controller->getPos()) + controller->getPosDeltaSetpoint();
    }
};
#endif
