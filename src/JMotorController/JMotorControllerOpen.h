#ifndef J_MOTOR_CONTROLLER_OPEN_H
#define J_MOTOR_CONTROLLER_OPEN_H
#include "JMotorController.h"
#include "JMotorControllerBasicOpen.h"
#include <Arduino.h>
#include <Derivs_Limiter.h>
/**
 * @brief  This class controls velocity and position of a motor without any encoder feedback
 */
class JMotorControllerOpen : public virtual JMotorController, public JMotorControllerBasicOpen {
protected:
    Derivs_Limiter dL;
    bool posMode;
    float position;
    float positionTarget;
    float velLimit;

public:
    JMotorControllerOpen(JMotorDriver& _driver, JMotorCompensator& _compensator, float _velLimit = INFINITY, float _accelLimit = INFINITY)
        : JMotorControllerBasicOpen(_driver, _compensator, _accelLimit)
        , dL(Derivs_Limiter(_velLimit, _accelLimit))
    {
        position = 0;
        positionTarget = 0;
        velLimit = max(_velLimit, (float)0.0);
    }

    void run()
    {
        if (posMode) {
            dL.setPositionVelocity(position, velocity);
            dL.setVelLimit(min(velLimit, getMaxVel()));
            dL.calc(positionTarget);
            position = dL.getPosition();
            JMotorControllerBasicOpen::setVel(dL.getVelocity());
        } else {
            position += velocity * (micros() - lastRunMicros) / 1000000.0;
            JMotorControllerBasicOpen::run();
        }
    }

    bool setPosTarget(float _posTarget)
    {
        if (posMode == false) {
            dL.resetTime();
        }

        if (position != _posTarget && velocity == 0) {
            velocity = ((_posTarget > position) ? getMinVel() : -getMinVel());
            dL.setVelocity(velocity);
        }
        posMode = true;
        if (positionTarget != _posTarget) {
            positionTarget = _posTarget;
            JMotorControllerOpen::run();
            return true;
        }
        JMotorControllerOpen::run();
        return false;
    }
    float getPosTarget()
    {
        return positionTarget;
    }
    float getPos()
    {
        return position;
    }
    float resetPos(float pos = 0)
    {
        if (posMode) {
            dL.setPosition(pos);
        }
        float temp = position;
        position = pos;
        return temp;
    }
    bool positioningMode()
    {
        return posMode;
    }

    //override basicOpen functions
    void setVel(float vel)
    {
        posMode = false;
        JMotorControllerBasicOpen::setVel(vel);
    }
    float setVelTarget(float vel)
    {
        posMode = false;
        return JMotorControllerBasicOpen::setVelTarget(vel);
    }
    /**
     * @brief  set maximum motor speed
     * @note   set to INFINITY to disable acceleration limiting
     * @param  _accelLimit: (float)
     */
    void setVelLimit(float _velLimit)
    {
        velLimit = max(_velLimit, (float)0.0);
        dL.setVelLimit(velLimit);
    }

    void setAccelLimit(float _accelLimit)
    {
        JMotorControllerBasicOpen::setAccelLimit(_accelLimit);
        dL.setAccelLimit(accelLimit);
    }
    /**
     * @brief  set velocity and acceleration limits for motor
     * @param  _vel: 
     * @param  _accel: 
     * @retval None
     */
    void setVelAccelLimits(float _vel, float _accel)
    {
        setVelLimit(_vel);
        setAccelLimit(_accel);
    }
};
#endif
