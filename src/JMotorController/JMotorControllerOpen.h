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
    bool posMode;
    bool smoothedMode;
    float position;
    float positionTarget;
    float posDelta;
    unsigned long lastRunOpenMicros;
    unsigned long minMotorPulseTime;
    unsigned long lastPosDeltaWrite;

public:
    /**
     * @brief  use functions from JMotorControllerOpen whenever possible, but the Derivs_Limiter object is public in case you need to change a setting like dL.setPreventGoingWrongWay()
     * @note   https://github.com/joshua-8/Derivs_Limiter
     */
    Derivs_Limiter dL;
    /**
     * @brief  Constructor for an object that controls a motor, and can smooth the motion of the motor and go approximately to a position.
     * @param  _driver: reference to a JMotorDriver
     * @param  _compensator: reference to a JMotorCompensator
     * @param  _velLimit: float, default INFINITY
     * @param  _accelLimit: float, default INFINITY
     * @param  _minMotorPulseTime: unsigned long, default 0. At low speeds in setPosDelta the motor cycles between min speed and stopped. What is the minimum pulse that makes the motor move, in MICROseconds?
     */
    JMotorControllerOpen(JMotorDriver& _driver, JMotorCompensator& _compensator, float _velLimit = INFINITY, float _accelLimit = INFINITY, unsigned long _minMotorPulseTime = 0)
        : JMotorControllerBasic(_driver, _compensator, _velLimit, _accelLimit)
        , dL(Derivs_Limiter(_velLimit, _accelLimit))
    {
        position = 0;
        positionTarget = 0;
        posMode = false;
        smoothedMode = true;
        posDelta = 0;
        lastRunOpenMicros = 0;
        minMotorPulseTime = _minMotorPulseTime;
        lastPosDeltaWrite = 0;
    }

    void run()
    {
        float time = (micros() - lastRunOpenMicros) / 1000000.0;
        lastRunOpenMicros = micros();
        if (time == 0) {
            return;
        }
        if (getEnable()) {
            if (posMode) {
                if (!smoothedMode) { // setPosSetpoint() mode
                    positionTarget += time * posDelta;
                    if (millis() - lastPosDeltaWrite > minMotorPulseTime) {
                        float posDeltaTime = (micros() - lastPosDeltaWrite) / 1000000.0;
                        lastPosDeltaWrite = micros();
                        if (position == positionTarget) {
                            JMotorControllerBasic::setVel(0, false);
                        } else if (abs(positionTarget - position) <= getMinVel() * posDeltaTime) { // TODO: no limit on how frequently this can happen
                            JMotorControllerBasic::setVel(0, false);
                        } else if (abs(positionTarget - position) < getMaxVel() * posDeltaTime) {
                            JMotorControllerBasic::setVel((positionTarget - position) / posDeltaTime, false);
                            position = positionTarget;
                        } else { // far away
                            JMotorControllerBasic::setVel((((positionTarget - position) > 0) ? getMaxVel() : -getMaxVel()), false);
                            position += velocity * posDeltaTime;
                        }
                    }

                } else { // setPosTarget() mode
                    dL.setPositionVelocity(position, velocity);
                    dL.setVelLimit(min(velLimit, getMaxVel()));
                    dL.calc(positionTarget);
                    position = dL.getPosition();
                    JMotorControllerBasic::setVel(dL.getVelocity(), true);
                }
            } else { // not pos mode
                JMotorControllerBasic::run();
                if (abs(velocity) > getMinVel()) {
                    position += velocity * time;
                }
            }
        }
    }

    bool setEnable(bool _enable)
    {
        if (!_enable && getEnable()) {
            posDelta = 0;
            positionTarget = position;
        }
        dL.resetTime();
        return JMotorControllerBasic::setEnable(_enable);
    }

    bool setPosTarget(float _posTarget, bool _run = true)
    {
        smoothedMode = true;
        if (posMode == false) {
            dL.resetTime();
        }

        if (position != _posTarget && velocity == 0) {
            dL.setVelocity((_posTarget > position) ? getMinVel() : -getMinVel());
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

    bool setPosDelta(float _posDelta, bool _run = true, bool _resetPos = false)
    {
        smoothedMode = false;
        posMode = true;
        bool ret = (posDelta != _posDelta);
        posDelta = _posDelta;
        if (_resetPos) {
            resetPos();
        }
        if (_run)
            JMotorControllerOpen::run();
        return ret;
    }

    float getPosTarget()
    {
        return positionTarget;
    }
    float getPosSetpoint()
    {
        return position;
    }
    float getPos()
    {
        return position;
    }
    float resetPos()
    {
        dL.setPosition(0);
        if (posMode)
            positionTarget -= position;
        float temp = position;
        position = 0;
        return temp;
    }

    bool isPosModeNotVelocity()
    {
        return posMode;
    }

    // override basicOpen functions
    void setVel(float vel, bool _run = true)
    {
        posMode = false;
        velocity = vel;
        velocityTarget = vel;
        if (_run)
            JMotorControllerOpen::run();
    }
    void setVelTarget(float vel, bool _run = true)
    {
        posMode = false;
        velocityTarget = vel;
        if (_run)
            JMotorControllerOpen::run();
    }
    void setVelLimit(float _velLimit)
    {
        JMotorControllerBasic::setVelLimit(_velLimit);
        dL.setVelLimit(velLimit);
    }
    void setAccelLimit(float _accelLimit)
    {
        JMotorControllerBasic::setAccelLimit(_accelLimit);
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

    /**
     * @brief At low speeds in setPosDelta the motor cycles between min speed and stopped. What is the minimum pulse that makes the motor move, in MICROseconds?
     * @param  _minMotorPulseTime: unsigned long
     */
    void setMinMotorPulseTime(unsigned long _minMotorPulseTime)
    {
        minMotorPulseTime = _minMotorPulseTime;
    }
};

#endif
