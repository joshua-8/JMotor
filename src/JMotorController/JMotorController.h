#ifndef J_MOTOR_CONTROLLER_H
#define J_MOTOR_CONTROLLER_H
#include "JMotorControllerBase.h"
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for classes which control velocity and position of a motor controlled by a JMotorDriver
 */
class JMotorController : public JMotorControllerBase {
protected:
    boolean posMode;
    /**
     * @brief  constructor
     * @param  _driver: reference to JMotorDriver
     */
    JMotorController(JMotorDriver& _driver, JMotorCompensator& _compensator)
        : JMotorControllerBase(_driver, _compensator)
    {
        posMode = false;
    }

public:
    /**
     * @brief  set target position
     * @note this function can be called repeatedly, or just once if run() is called frequently
     * @param  pt: (float) target position
     * @retval (boolean) did the  target change
     */
    virtual boolean setPosTarget(float pt);
    /**
     * @brief  get position set as target
     * @retval (float)
     */
    virtual float getPosTarget();
    /**
     * @brief  get what position the motor is currently at
     * @retval (float)
     */
    virtual float getPos();
    /**
     * @brief  reset what position the controller thinks it's in
     * @note  make sure to use this frequently enough that the limited precision of floats doesn't become a problem. don't drive too far without resetting
     * @param  pos: (float) value to reset position to, default: 0
     * @retval (float) returns old position
     */
    virtual float resetPos(float pos = 0);
    /**
     * @brief  true if driving to target position, false if under velocity control
     * @retval (boolean)
     */
    boolean getPosMode()
    {
        return posMode;
    }
};
#include "JMotorController.h"
#endif
