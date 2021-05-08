#ifndef J_MOTOR_CONTROLLER_H
#define J_MOTOR_CONTROLLER_H
#include "JMotorControllerBase.h"
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for classes which control velocity and position of a motor controlled by a JMotorDriver
 */
class JMotorController : public virtual JMotorControllerBase {
public:
    /**
     * @brief  set target position
     * @note this function can be called repeatedly, or just once if run() is called frequently
     * @param  _posTarget: (float) target position
     * @retval (bool) did the target change
     */
    virtual bool setPosTarget(float _posTarget) = 0;

    /**
     * @brief  get position set as target
     * @retval (float)
     */
    virtual float getPosTarget() = 0;

    /**
     * @brief  get what position the motor is currently at
     * @retval (float)
     */
    virtual float getPos() = 0;

    /**
     * @brief  reset what position the controller thinks it's in
     * @note   don't drive too far without resetting so that the limited precision of floats doesn't become a problem.
     * @param  pos: (float) value to reset position to, default: 0
     * @retval (float) returns old position
     */
    virtual float resetPos(float pos = 0) = 0;

    /**
     * @brief  true if driving to target position, false if under velocity control
     * @retval (bool)
     */
    virtual bool positioningMode() = 0;
};
// #include "JMotorControllerClosed.h"
#include "JMotorControllerOpen.h"
#endif
