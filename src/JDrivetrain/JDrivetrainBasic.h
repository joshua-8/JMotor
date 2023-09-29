#ifndef J_DRIVETRAIN_BASIC_H
#define J_DRIVETRAIN_BASIC_H
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 * @brief  interface for a drivetrain with only simple velocity control
 */
class JDrivetrainBasic {
public:
    /**
     * @brief  call this in loop. it calls any encoder or motor update functions
     */
    virtual void run() = 0;
    /**
     * @brief  enables or disables movement
     * @param  _enable: (bool) true=enabled false=movement disabled
     * @retval did the state change
     */
    virtual bool setEnable(bool _enable) = 0;
    /**
     * @brief  enables movement
     * @retval did the state change
     */
    virtual bool enable() = 0;
    /**
     * @brief  disables movement
     * @retval did the state change
     */
    virtual bool disable() = 0;
    /**
     * @brief  gets the current state (enabled or disabled)
     * @retval (bool) true=enabled false=movement disabled
     */
    virtual bool getEnable() = 0;
    /**
     * @brief  sets a velocity for the drivetrain to move at
     * @param  _vel: (JTwoDTransform) velocity to move at
     * @param  _run: (bool, default=true) if true setVel will call run() for you, if false you must call it yourself
     * @retval None
     */
    virtual void setVel(JTwoDTransform _vel, bool _run = false) = 0;
    /**
     * @brief  returns the approximate maximum velocity of the drivetrain, if the drivetrain is told to move at max velocity, it should set the motors to full power.
     * @note   depends on how well calibrated the motor compensators are
     * @retval (JTwoDTransform) maximum velocity (in whatever units your drivetrain and motors are set up for)
     */
    virtual JTwoDTransform getMaxVel() = 0;
};
#endif // J_DRIVETRAIN_BASIC_H
