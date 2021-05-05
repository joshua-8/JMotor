#ifndef J_MOTOR_CONTROLLER_BASE_H
#define J_MOTOR_CONTROLLER_BASE_H
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for classes which control velocity of a motor controlled by a JMotorDriver
 */
class JMotorControllerBase {
public:
    /**
     * @brief  set velocity for controller
     * @note  this function can be called repeatedly, or just once if run() is called frequently
     * @param  vel: (float)
     */
    virtual void setVel(float vel);

    /**
     * @brief  set target velocity for controller (acceleration limited)
     * @note  this function can be called repeatedly, or just once if run() is called frequently
     * @param  vel: (float) target velocity
     * @retval (float) current velocity
     */
    virtual float setVelTarget(float vel);

    /**
     * @brief  get current velocity of motor
     * @retval (float)
     */
    virtual float getVel();

    /**
     * @brief  returns the value the motor driver is being set to
     * @retval (float)
     */
    virtual float getDriverSetVal();

    /**
     * @brief  update driver
     * @note   call this function as frequently as possible
     */
    virtual void run();

    /**
     * @brief  change whether motor controller is enabled
     * @param  _enable: (bool)
     * @retval (bool) true if state changed
     */
    virtual bool setEnable(bool _enable);

    /**
     * @brief  true if motor driver is being set to within its range, false if driver is at a maximum
     * @retval (bool)
     */
    virtual bool isDriverInRange();

    /**
     * @brief  enable motor controller
     * @retval (bool) true if state changed
     */
    virtual bool enable();

    /**
     * @brief  disable motor controller 
     * @retval (bool) true if state changed
     */
    virtual bool disable();

    /**
     * @brief  if getDriverSetVal goes below this, isDriverInRange will go false
     * @retval (float)
     */
    virtual float getDriverMinRange();

    /**
     * @brief  if getDriverSetVal exceeds this, isDriverInRange will go false
     * @retval (float)
     */
    virtual float getDriverMaxRange();

    /**
     * @brief  is the controller enabled
     * @retval (bool)
     */
    virtual bool getEnabled();

    /**
     * @brief  How fast of a motor speed setting would get adjusted to full motor power
     * @note   may change as battery voltage changes
     * @retval  (float)
     */
    virtual float getMaxVel();
};
#include "JMotorControllerBasicOpen.h"
#endif
