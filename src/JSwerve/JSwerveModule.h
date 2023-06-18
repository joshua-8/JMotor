#ifndef JSWERVE_MODULE_H
#define JSWERVE_MODULE_H
#include <Arduino.h>
/**
 * @brief  defines a common interface for swerve modules
 */
class JSwerveModule {
public:
    /**
     * @brief  set speed and angle setpoints for wheel
     * @param  speed: (float) speed for wheel
     * @param  angle: (float) angle in degrees
     * @param  run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     * @retval None
     */
    virtual void set(float _speed, float _angle, bool _run = true);
    /**
     * @brief  get angle measurement from wheel
     * @retval (float) angle in degrees
     */
    virtual float getAngle();
    /**
     * @brief  maximum drive speed
     */
    virtual float getMaxSpeed();
    /**
     * @brief  zeros the encoder's counter of full turns, leaving only the fractional turn
     * @retval (long) previous turn counter value
     */
    virtual long zeroTurnCounter();
    /**
     * @brief  set enable state of swerve module motors
     * @param  _enable: (bool) true = enable module, false = disable module
     * @retval (bool) true if state changed, false if state already set
     */
    virtual bool setEnable(bool _enable);
    /**
     * @brief  get enable state of module
     * @retval (bool) true = motors enabled, false = motors disabled
     */
    virtual bool getEnable();
    /**
     * @brief  enable motor
     * @note equivalent to setEnable(true)
     * @retval (bool) true if state changed, false if state already set
     */
    bool enable()
    {
        return setEnable(true);
    }

    /**
     * @brief  disable motor
     * @note equivalent to setEnable(false)
     * @retval (bool) true if state changed, false if state already set
     */
    bool disable()
    {
        return setEnable(false);
    }
    /**
     * @brief  call this in loop() so that the swerve module can update its motors and angle control loop
     */
    virtual void run();
};

#include "JSwerveModuleDifferential.h"

#endif // JSWERVE_MODULE_H
