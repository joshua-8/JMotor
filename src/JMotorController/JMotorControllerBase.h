#ifndef J_MOTOR_CONTROLLER_BASE_H
#define J_MOTOR_CONTROLLER_BASE_H
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for classes which control velocity of a motor controlled by a JMotorDriver
 */
class JMotorControllerBase {
public:
    /**
     * @brief  set maximum rate that motor speed can be changed at
     * @note   set to INFINITY to disable acceleration limiting
     * @param  _accelLimit: (float)
     */
    virtual void setAccelLimit(float _accelLimit) = 0;

    /**
     * @brief  set maximum motor speed
     * @note   set to INFINITY to disable velocity limiting
     * @param  _velLimit: (float)
     */
    virtual void setVelLimit(float _velLimit) = 0;

    /**
     * @brief  set velocity for controller
     * @note  this function can be called repeatedly, or just once if run() is called frequently
     * @param  vel: (float)
     * @param  _run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     */
    virtual void setVel(float vel, bool _run = true) = 0;

    /**
     * @brief  set target velocity for controller (acceleration limited)
     * @note  this function can be called repeatedly, or just once if run() is called frequently
     * @param  vel: (float) target velocity
     * @param  _run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     */
    virtual void setVelTarget(float vel, bool _run = true) = 0;

    /**
     * @brief  get target velocity for controller
     * @retval (float) current velocity target
     */
    virtual float getVelTarget() = 0;

    /**
     * @brief  get current velocity of motor
     * @retval (float)
     */
    virtual float getVel() = 0;

    /**
     * @brief  returns the value the motor driver is being set to
     * @retval (float)
     */
    virtual float getDriverSetVal() = 0;

    /**
     * @brief  update driver
     * @note   call this function as frequently as possible
     */
    virtual void run() = 0;

    /**
     * @brief  change whether motor controller is enabled
     * @param  _enable: (bool)
     * @note  velocity related variables are reset to zero on disable. you can set velocity then enable
     * @retval (bool) true if state changed
     */
    virtual bool setEnable(bool _enable) = 0;

    /**
     * @brief  true if motor driver is being set to within its range, false if driver is at a maximum
     * @retval (bool)
     */
    virtual bool isDriverInRange() = 0;

    /**
     * @brief  enable motor controller
     * @retval (bool) true if state changed
     */
    virtual bool enable() = 0;

    /**
     * @brief  disable motor controller 
     * @retval (bool) true if state changed
     */
    virtual bool disable() = 0;

    /**
     * @brief  if getDriverSetVal goes below this, isDriverInRange will go false
     * @retval (float)
     */
    virtual float getDriverMinRange() = 0;

    /**
     * @brief  if getDriverSetVal exceeds this, isDriverInRange will go false
     * @retval (float)
     */
    virtual float getDriverMaxRange() = 0;

    /**
     * @brief  is the controller enabled
     * @retval (bool)
     */
    virtual bool getEnable() = 0;

    /**
     * @brief  How fast of a motor speed setting would get adjusted to full motor power
     * @note   may change as battery voltage changes
     * @retval  (float)
     */
    virtual float getMaxVel() = 0;

    /**
     * @brief  slowest speed motor can go
     */
    virtual float getMinVel() = 0;

    /**
     * @brief  set what fraction of driverRange can be used
     * @param  _driverRangeAmount: (float)
     */
    virtual void setMaxDriverRangeAmount(float _driverRangeAmount) = 0;

    /**
     * @brief  returns value of accelLimit
     * @retval  (float)
     */
    virtual float getAccelLimit() = 0;

    /**
     * @brief  returns value of velLimit
     * @retval  (float)
     */
    virtual float getVelLimit() = 0;
};
#include "JMotorControllerBasic.h"
#endif
