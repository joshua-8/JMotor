#ifndef J_MOTOR_CONTROLLER_OPEN_H
#define J_MOTOR_CONTROLLER_OPEN_H
#include "JMotorController.h"
#include "JMotorControllerBasic.h"
#include <Arduino.h>
#include <Derivs_Limiter.h>
/**
 * @brief  This class controls velocity and position of a motor without any encoder feedback
 */
class JMotorControllerOpen : public virtual JMotorController, public JMotorControllerBasic {
protected:
    Derivs_Limiter dL;
    bool posMode;
    bool smoothedMode;
    float position;
    float positionTarget;
    float velLimit;
    float posDelta;

public:
    JMotorControllerOpen(JMotorDriver& _driver, JMotorCompensator& _compensator, float _velLimit = INFINITY, float _accelLimit = INFINITY)
        : JMotorControllerBasic(_driver, _compensator, _accelLimit)
        , dL(Derivs_Limiter(_velLimit, _accelLimit))
    {
        position = 0;
        positionTarget = 0;
        velLimit = max(_velLimit, (float)0.0);
        posMode = false;
        smoothedMode = true;
        posDelta = 0;
    }

    void run()
    {
        if (posMode) {
            if (!smoothedMode) { //setPosSetpoint() mode
                float time = (micros() - lastRunMicros) / 1000000.0;
                positionTarget += time * posDelta;
                if (position == positionTarget) {
                    JMotorControllerBasic::setVel(0);
                } else if (abs(positionTarget - position) <= getMinVel() * time) {
                    JMotorControllerBasic::setVel(0);
                } else if (abs(positionTarget - position) < getMaxVel() * time) {
                    JMotorControllerBasic::setVel((positionTarget - position) / time);
                    position = positionTarget;
                } else { //far away
                    JMotorControllerBasic::setVel((((positionTarget - position) > 0) ? getMaxVel() : -getMaxVel()), false);
                    position += velocity * time;
                    JMotorControllerBasic::run();
                }
            } else { //setPosTarget() mode
                dL.setPositionVelocity(position, velocity);
                dL.setVelLimit(min(velLimit, getMaxVel()));
                dL.calc(positionTarget);
                position = dL.getPosition();
                JMotorControllerBasic::setVel(dL.getVelocity());
            }
        } else {
            if (abs(velocity) > getMinVel()) {
                position += velocity * (micros() - lastRunMicros) / 1000000.0;
            }
            JMotorControllerBasic::run();
        }
    }

    bool setPosTarget(float _posTarget, bool _run = true)
    {
        smoothedMode = true;
        if (posMode == false) {
            dL.resetTime();
        }

        if (position != _posTarget && velocity == 0) {
            dL.setVelocity((_posTarget > position) ? getMaxVel() : -getMaxVel());
        }
        posMode = true;
        if (positionTarget != _posTarget) {
            positionTarget = _posTarget;
            if (_run)
                JMotorControllerOpen::run();
            return true;
        }
        if (_run)
            JMotorControllerOpen::run();
        return false;
    }
    bool setPosSetpoint(float _posSetpoint, bool _run = true)
    {
        smoothedMode = false;
        posMode = true;
        posDelta = 0;
        if (positionTarget != _posSetpoint) {
            positionTarget = _posSetpoint;
            if (_run)
                JMotorControllerOpen::run();
            return true;
        }
        if (_run)
            JMotorControllerOpen::run();
        return false;
    }

    void setPosDelta(float _posDelta, bool _run = true)
    {
        smoothedMode = false;
        posMode = true;
        posDelta = _posDelta;
        if (_run)
            JMotorControllerOpen::run();
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
        positionTarget += (pos - position);
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
        velocity = vel;
        velocityTarget = vel;
        JMotorControllerOpen::run();
    }
    float setVelTarget(float vel)
    {
        posMode = false;
        velocityTarget = vel;
        JMotorControllerOpen::run();
        return velocity;
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

    float setAccelLimit(float _accelLimit)
    {
        JMotorControllerBasic::setAccelLimit(_accelLimit);
        dL.setAccelLimit(accelLimit);
        return accelLimit;
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
