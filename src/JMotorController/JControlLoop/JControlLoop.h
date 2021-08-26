#ifndef J_CONTROL_LOOP_H
#define J_CONTROL_LOOP_H
class JMotorControllerClosed; //"forward declaration"
/**
 * @brief  interface for control loop used by JMotorControllerClosed
 */
class JControlLoop {
public:
    virtual float calc(JMotorControllerClosed* controller) = 0;
    virtual void resetTime() = 0;
};
#include "JMotorController/JMotorControllerClosed.h"

#include "JControlLoopBasic.h"

#endif
