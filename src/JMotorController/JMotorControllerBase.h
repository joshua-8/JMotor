#ifndef J_MOTOR_CONTROLLER_BASE_H
#define J_MOTOR_CONTROLLER_BASE_H
#include "JMotorCompensator/JMotorCompensator.h"
#include "JMotorDriver/JMotorDriver.h"
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for classes which control velocity of a motor controlled by a JMotorDriver
 */
class JMotorControllerBase {
protected:
    JMotorDriver& driver;
    boolean driverInRange;
    JMotorCompensator& compensator;

protected:
    /**
     * @brief  constructor
     * @param  _driver: reference to JMotorDriver
     */
    JMotorControllerBase(JMotorDriver& _driver, JMotorCompensator& _compensator)
        : driver(_driver)
        , compensator(_compensator)
    {
        driverInRange = true;
    }

public:
    /**
     * @brief  set target velocity for controller
     * @note  this function can be called repeatedly, or just once if run() is called frequently
     * @param  vel: (float)
     */
    virtual void setVel(float vel);
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
     * @param  _enable: (boolean)
     * @retval (boolean) true if state changed
     */
    virtual boolean setEnable(boolean _enable);
    /**
     * @brief  true if motor driver is being set to within its range, false if driver is at a maximum
     * @retval (boolean)
     */
    boolean isDriverInRange()
    {
        return driverInRange;
    }
    /**
     * @brief  enable motor controller
     * @retval (boolean) true if state changed
     */
    boolean enable()
    {
        return setEnable(true);
    }
    /**
     * @brief  disable motor controller 
     * @retval (boolean) true if state changed
     */
    boolean disable()
    {
        return setEnable(false);
    }
    /**
     * @brief  if getDriverSetVal goes below this, isDriverInRange will go false
     * @retval (float)
     */
    float getDriverMinRange()
    {
        return driver.getMinRange();
    }
    /**
     * @brief  if getDriverSetVal exceeds this, isDriverInRange will go false
     * @retval (float)
     */
    float getDriverMaxRange()
    {
        return driver.getMaxRange();
    }
};
#include "JMotorController.h"
#include "JMotorControllerBasicOpen.h"
#endif
