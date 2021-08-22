#ifndef J_MOTOR_CONTROLLER_CLOSED_H
#define J_MOTOR_CONTROLLER_CLOSED_H
#include "Derivs_Limiter.h"
#include "JControlLoop/JControlLoop.h"
#include "JEncoder/JEncoder.h"
#include "JMotorCompensator/JMotorCompensator.h"
#include "JMotorController.h"
#include "JMotorControllerBasic.h"
#include <Arduino.h>
/**
 * @brief  closed loop (uses encoder) motor controller
 */
class JMotorControllerClosed : public virtual JMotorController, public virtual JMotorControllerBase {
protected:
    JMotorDriver& driver;
    JMotorCompensator& compensator;
    JEncoder& encoder;

    float velLimit;
    float accelLimit;
    float setVal;
    bool driverInRange;
    bool open;
    unsigned long lastRunMicros;
    float velSetpoint;
    float velSetpointTarget;
    bool posMode;
    float posSetpoint;
    bool smoothed;
    float posDeltaSetpoint;
    float posDeltaSetpointTarget;
    bool limitSetpointDistFromCurrent;
    float distFromSetpointLimit;
    float lastPosSetpoint;
    float time;

public:
    /**
     * Derivs_Limiter object, used for smoothly driving to position
     * */
    Derivs_Limiter posSetpointSmoother;
    JControlLoop& controlLoop;

    /**
     * @brief  Constructor for closed loop motor controller
     * @param  _driver: (JMotorDriver&)
     * @param  _compensator: (JMotorCompensator&)
     * @param  _encoder: (JEncoder)
     * @param  _controlLoop: (JControlLoop)
     * @param  _velLimit: (float) default=INFINITY, 
     * @param  _accelLimit: (float) default=INFINITY,
     * @param  _distFromSetpointLimit: (float)= 1.0,
     * @param  _preventGoingWrongWay: (bool) default=true,
     * @param  _maxStoppingAccel: (float) default=INFINITY,
     */
    JMotorControllerClosed(JMotorDriver& _driver, JMotorCompensator& _compensator, JEncoder& _encoder, JControlLoop& _controlLoop, float _velLimit = INFINITY, float _accelLimit = INFINITY, float _distFromSetpointLimit = 1, bool _preventGoingWrongWay = true, float _maxStoppingAccel = INFINITY)
        : driver(_driver)
        , compensator(_compensator)
        , encoder(_encoder)
        , posSetpointSmoother(Derivs_Limiter(_velLimit, _accelLimit))
        , controlLoop(_controlLoop)
    {
        posSetpointSmoother.setPreventGoingWrongWay(_preventGoingWrongWay);
        posSetpointSmoother.setMaxStoppingAccel(_maxStoppingAccel);
        compensator.setDriverRange(driver.getMaxRange());
        velLimit = max(_velLimit, (float)0.0);
        accelLimit = max(_accelLimit, (float)0.0);
        open = true;
        setVal = 0;
        lastRunMicros = micros();
        posMode = false;
        posSetpoint = 0;
        lastPosSetpoint = 0;
        smoothed = false;
        posDeltaSetpoint = 0;
        posDeltaSetpointTarget = 0;
        velSetpointTarget = 0;
        velSetpoint = 0;
        limitSetpointDistFromCurrent = false;
        distFromSetpointLimit = max(_distFromSetpointLimit, (float)0.0);
        time = 0;
    }
    void run()
    {
        encoder.run();
        time = (micros() - lastRunMicros) / 1000000.0;
        if (time == 0)
            return;
        lastRunMicros = micros();
        if (getEnable()) {
            if (open) {
                if (velSetpoint != velSetpointTarget) {
                    if (posSetpointSmoother.getPreventGoingWrongWay() && velSetpointTarget != 0 && velSetpoint != 0 && ((velSetpointTarget > 0) != (velSetpoint > 0))) {
                        velSetpoint = 0;
                    }
                    velSetpoint += constrain(velSetpointTarget - velSetpoint, -accelLimit * time, accelLimit * time);
                }
                velSetpoint = constrain(velSetpoint, -compensator.getMaxVel() * (getDriverMinRange() < 0), compensator.getMaxVel() * (getDriverMaxRange() > 0));
                velSetpoint = constrain(velSetpoint, -velLimit, velLimit);
                setVal = compensator.compensate(velSetpoint);

                if (!encoder.hasDirection()) {
                    encoder.setRev((setVal <= 0));
                }

                driverInRange = driver.set(setVal);
                posSetpoint = encoder.getPos();
                posDeltaSetpoint = velSetpoint;
            } else { //closed loop
                if (posMode) {
                    if (smoothed) { //setPosTarget
                        posSetpointSmoother.setPosition(posSetpoint);
                        posSetpoint = posSetpointSmoother.calc();
                        posDeltaSetpoint = posSetpointSmoother.getVelocity();
                        if (limitSetpointDistFromCurrent) {
                            posSetpoint = constrain(posSetpoint, encoder.getPos() - distFromSetpointLimit, encoder.getPos() + distFromSetpointLimit);
                            posSetpointSmoother.setPosition(posSetpoint);
                        }
                    } else { //setPosSetpoint
                        posDeltaSetpoint = (posSetpoint - lastPosSetpoint) / time;
                        //nothing to do here, posSetpoint was set
                    }
                } else { //posDelta
                    if (posDeltaSetpoint != posDeltaSetpointTarget) {
                        if (posSetpointSmoother.getPreventGoingWrongWay() && posDeltaSetpointTarget != 0 && posDeltaSetpoint != 0 && ((posDeltaSetpointTarget > 0) != (posDeltaSetpoint > 0))) {
                            posDeltaSetpoint = 0; //prevent going wrong way
                        }
                        posDeltaSetpointTarget = constrain(posDeltaSetpointTarget, -velLimit, velLimit);
                        posDeltaSetpointTarget = constrain(posDeltaSetpointTarget, -compensator.getMaxVel() * (getDriverMinRange() < 0), compensator.getMaxVel() * (getDriverMaxRange() > 0));
                        posDeltaSetpoint += constrain(posDeltaSetpointTarget - posDeltaSetpoint, -accelLimit * time, accelLimit * time);
                    }
                    posSetpoint += posDeltaSetpoint * time;
                    if (limitSetpointDistFromCurrent)
                        posSetpoint = constrain(posSetpoint, encoder.getPos() - distFromSetpointLimit, encoder.getPos() + distFromSetpointLimit);
                }
                //MAKE MOTOR GO TO posSetpoint, by setting velSetpointTarget

                velSetpointTarget = controlLoop.calc(this);

                lastPosSetpoint = posSetpoint;

                velSetpoint = velSetpointTarget; //open loop uses velSetpointTarget and velsetpoint for acceleration, but closed loop has posDeltaSetpointTarget and posDeltaSetpoint, so just set them equal

                setVal = compensator.compensate(velSetpoint);

                if (!encoder.hasDirection()) {
                    encoder.setRev((setVal < 0));
                }

                driverInRange = driver.set(setVal);
            } //end of closed loop mode
        } //enabled
    }

    void setVel(float vel, bool _run = true)
    {
        setPosDelta(vel, false, false);
        limitSetpointDistFromCurrent = true;
        if (_run)
            run();
    }
    void setVelTarget(float vel, bool _run = true)
    {
        setAccelPosDelta(vel, false, false);
        limitSetpointDistFromCurrent = true;
        if (_run)
            run();
    }
    bool setPosTarget(float _posTarget, bool _run = true)
    {
        limitSetpointDistFromCurrent = false;
        if (open)
            controlLoop.resetTime();
        open = false;
        if (posMode == false) {
            posSetpointSmoother.resetTime();
            posSetpointSmoother.setVelocity(velSetpoint);
        }
        bool ret = posSetpointSmoother.getTarget() != _posTarget;
        posMode = true;
        smoothed = true;
        posSetpointSmoother.setTarget(_posTarget);
        if (_run)
            run();
        return ret; //did the target change?
    }
    bool setPosTargetStallable(float _posTarget, bool _run = true)
    {
        limitSetpointDistFromCurrent = true;
        if (open)
            controlLoop.resetTime();
        open = false;
        if (posMode == false) {
            posSetpointSmoother.resetTime();
            posSetpointSmoother.setVelocity(velSetpoint);
        }
        bool ret = posSetpointSmoother.getTarget() != _posTarget;
        posMode = true;
        smoothed = true;
        posSetpointSmoother.setTarget(_posTarget);
        if (_run)
            run();
        return ret; //did the target change?
    }
    bool setPosSetpoint(float _posSetpoint, bool _run = true)
    {
        if (open)
            controlLoop.resetTime();
        open = false;
        posMode = true;
        smoothed = false;
        bool ret = posSetpoint != _posSetpoint;
        posSetpoint = _posSetpoint;
        if (_run)
            run();
        return ret; //did the setpoint change?
    }
    bool setPosDelta(float _posDelta, bool _run = true, bool _resetPos = false)
    {
        if (open)
            controlLoop.resetTime();
        open = false;
        posMode = false;
        limitSetpointDistFromCurrent = false;
        bool ret = posDeltaSetpointTarget != _posDelta;
        posDeltaSetpoint = _posDelta;
        posDeltaSetpointTarget = _posDelta;
        if (_resetPos)
            resetPos();
        if (_run)
            run();
        return ret; //did the setting change?
    }
    void setAccelPosDelta(float _posDelta, bool _run = true, bool _resetPos = false)
    {
        if (open == true || posMode == true) {
            posDeltaSetpoint = velSetpoint;
        }
        if (open)
            controlLoop.resetTime();
        open = false;
        posMode = false;
        limitSetpointDistFromCurrent = false;
        posDeltaSetpointTarget = _posDelta;
        if (_resetPos)
            resetPos();
        if (_run)
            run();
    }
    /**
     * @brief  set velocity without closed loop control
     * @param  vel: (float) velocity to set
     * @param  _run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     */
    void setOpenVel(float vel, bool _run = true)
    {
        velSetpointTarget = vel;
        velSetpoint = vel;
        posMode = false;
        open = true;
        if (_run)
            run();
    }
    /**
     * @brief  set velocity without closed loop control, with limited acceleration
     * @param  vel: (float) velocity to approach
     * @param  _run: (bool) default:true, true = call run() in this function, false = you'll call run() yourself
     */
    void setOpenVelTarget(float vel, bool _run = true)
    {
        velSetpointTarget = vel;
        open = true;
        posMode = false;
        if (_run)
            run();
    }

    float getSpeedError()
    {
        return (velSetpoint - encoder.getVel()) / velSetpoint;
    }

    float getPosTarget()
    {
        if (!open && posMode && smoothed) {
            return posSetpointSmoother.getTarget();
        }
        return posSetpoint;
    }

    float getPosSetpoint()
    {
        return posSetpoint;
    }

    float getVelTarget()
    {
        if (open)
            return velSetpointTarget;
        else
            return posDeltaSetpointTarget;
    }

    float getPosDeltaSetpoint()
    {
        return posDeltaSetpoint;
    }

    float getLastPosSetpoint()
    {
        return lastPosSetpoint;
    }

    float getTime()
    {
        return time;
    }

    float getDistFromSetpointLimit()
    {
        return distFromSetpointLimit;
    }

    void setDistFromSetpointLimit(float _distFromSetpointLimit)
    {
        distFromSetpointLimit = max(_distFromSetpointLimit, (float)0.0);
    }

    /**
     * @brief  returns how fast the motor driver is told to move
     * @note  use getVel for actual encoder reading
     * @retval  (float)
     */
    float getVelSetpoint()
    {
        return velSetpoint;
    }
    float getVel()
    {
        return encoder.getVel();
    }
    float getPos()
    {
        return encoder.getPos();
    }
    float resetPos()
    {
        float oldDist = encoder.getPos();
        encoder.zeroCounter();
        posSetpoint -= oldDist;
        lastPosSetpoint -= oldDist;
        posSetpointSmoother.setPosition(0);
        posSetpointSmoother.setTarget(posSetpointSmoother.getTarget() - oldDist);

        return oldDist;
    }
    bool isPosModeNotVelocity()
    {
        return posMode;
    }
    bool isClosedLoop()
    {
        return !open;
    }
    void setAccelLimit(float _accelLimit)
    {
        accelLimit = max(_accelLimit, (float)0.0);
        posSetpointSmoother.setAccelLimit(accelLimit);
    }
    void setVelLimit(float _velLimit)
    {
        velLimit = max(_velLimit, (float)0.0);
        posSetpointSmoother.setVelLimit(velLimit);
    }

    float getAccelLimit()
    {
        return accelLimit;
    }
    float getVelLimit()
    {
        return velLimit;
    }

    void setPreventGoingWrongWay(bool _preventGoingWrongWay)
    {
        posSetpointSmoother.setPreventGoingWrongWay(_preventGoingWrongWay);
    }

    void setMaxStoppingAccel(float _maxStoppingAccel)
    {
        posSetpointSmoother.setMaxStoppingAccel(_maxStoppingAccel);
    }

    float getDriverSetVal()
    {
        return setVal;
    }

    bool setEnable(bool _enable)
    {
        if (_enable == false) {
            posSetpointSmoother.setVelocity(0);
            velSetpoint = 0;
        }
        if (_enable == true && !getEnable()) { //enabling
            lastRunMicros = micros();
            controlLoop.resetTime();
            posSetpointSmoother.resetTime();
        }
        return driver.setEnable(_enable);
    }
    void setMaxDriverRangeAmount(float _driverRangeAmount)
    {
        compensator.setMaxDriverRangeAmount(_driverRangeAmount);
    }

    bool isDriverInRange()
    {
        return driverInRange;
    }

    bool enable()
    {
        return setEnable(true);
    }

    bool disable()
    {
        return setEnable(false);
    }

    float getDriverMinRange()
    {
        return driver.getMinRange() * compensator.getMaxDriverRangeAmount();
    }

    float getDriverMaxRange()
    {
        return driver.getMaxRange() * compensator.getMaxDriverRangeAmount();
    }

    bool getEnable()
    {
        return driver.getEnable();
    }

    float getMaxVel()
    {
        return compensator.getMaxVel();
    }

    float getMinVel()
    {
        return compensator.getMinVel();
    }
};
#endif
