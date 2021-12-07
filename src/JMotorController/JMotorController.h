#ifndef J_MOTOR_CONTROLLER_H
#define J_MOTOR_CONTROLLER_H
#include "JMotorControllerBasic.h"
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for classes which control velocity and position of a motor controlled by a JMotorDriver
 */
class JMotorController : public virtual JMotorControllerBase {
public:
    /**
     * @brief  set target position (motor drives towards position, following acceleration profile)
     * @param  _posTarget: (float) target position
     * @param  _run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     * @retval (bool) did the target change?
     */
    virtual bool setPosTarget(float _posTarget, bool _run = true) = 0;

    /**
     * @brief  set position for motor to drive towards position as fast as possible (setpoint for control loop if available)
     * @note  run() needs to be called in order for the motor to move
     * @param  _posSetpoint: (float) position setpoint
     * @param  _run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     * @retval (bool) did the setpoint change?
     */
    virtual bool setPosSetpoint(float _posSetpoint, bool _run = true) = 0;

    /**
     * @brief  alternative method for setting velocity that uses setPosSetpoint
     * @note  handles velocities below getMinVel(), and may have better distance accuracy, doesn't smooth velocity
     * @param  _posDelta: (float) basically velocity
     * @param  _resetPos: (bool) default=false, keep setting current position to zero
     * @param  _run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     * @retval  (bool) did the setting change?
     */
    virtual bool setPosDelta(float _posDelta, bool _run = true, bool _resetPos = false) = 0;

    /**
     * @brief  get position set as target (for smoothed position setting mode)
     * @retval (float)
     */
    virtual float getPosTarget() = 0;

    /**
     * @brief  get position controller is currently trying to get to
     * @retval (float)
     */
    virtual float getPosSetpoint() = 0;

    /**
     * @brief  get what position the motor is currently at
     * @retval (float)
     */
    virtual float getPos() = 0;

    /**
     * @brief  reset what position the controller thinks it's in
     * @note   don't drive too far without resetting this so that the limited precision of floats doesn't become a problem.
     * @retval (float) returns old position
     */
    virtual float resetPos() = 0;
    /**
     * @brief  set maximum motor speed
     * @note   set to INFINITY to disable limit
     * @param  _velLimit: (float)
     */
    virtual void setVelLimit(float _velLimit) = 0;

    /**
     * @brief  true if controller is in position target mode, false if in velocity  mode
     * @retval (bool)
     */
    virtual bool isPosModeNotVelocity() = 0;
};
#include "JControlLoop/JControlLoop.h"
#include "JMotorControllerClosed.h"
#include "JMotorControllerOpen.h"
#endif
