#ifndef J_CONTROL_LOOP_H
#define J_CONTROL_LOOP_H
class JMotorControllerClosed; //"forward declaration"
/**
 * @brief  interface for control loop used by JMotorControllerClosed
 */
class JControlLoop {
protected:
    float error;
    float ctrlLoopOut;
    float result;

public:
    virtual float calc(JMotorControllerClosed* controller) = 0;
    virtual void resetTime() = 0;
    /**
     * @brief  setpoint-position
     * @retval (float)
     */
    float getError()
    {
        return error;
    }
    /**
     * @brief  most recent value calculated by control loop in response to error (but not including feedforward)
     * For example, if the control loop is a proportional loop, this function returns P * error
     * @retval (float)
     */
    float getCtrlLoopOut()
    {
        return result;
    }
    /**
     * @brief  most recent value returned by calc()
     * overall output to motor
     * @retval (float)
     */
    float getResult()
    {
        return result;
    }
};
#include "JMotorController/JMotorControllerClosed.h"

#include "JControlLoopBasic.h"

#endif
