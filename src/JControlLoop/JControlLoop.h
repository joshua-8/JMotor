#ifndef J_CONTROL_LOOP_H
#define J_CONTROL_LOOP_H
class JMotorControllerClosed; //"forward declaration" ?
class JControlLoop {
public:
    virtual float calc(JMotorControllerClosed* controller) = 0;
};
#include "JMotorController/JMotorControllerClosed.h"

#include "JControlLoopBasic.h"

#endif
