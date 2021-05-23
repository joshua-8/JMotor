#ifndef J_MOTOR_CONTROLLER_CLOSED_H
#define J_MOTOR_CONTROLLER_CLOSED_H
#include "JEncoder/JEncoder.h"
#include "JMotorController.h"
#include "JMotorControllerBasic.h"
#include <Arduino.h>
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
    float timeFromSetpointLimit;

public:
    /**
     * Derivs_Limiter object, used for smoothly driving to position, 
     * */
    Derivs_Limiter posSetpointSmoother;
    //JControlLoop
    JMotorControllerClosed(JMotorDriver& _driver, JMotorCompensator& _compensator, JEncoder& _encoder, /*JControlLoop& _controllLoop,*/ float _velLimit = INFINITY, float _accelLimit = INFINITY, bool _preventGoingWrongWay = true, float _maxStoppingAccel = 1.5, float _timeFromSetpointLimit = 1.0)
        : driver(_driver)
        , compensator(_compensator)
        , encoder(_encoder)
        , posSetpointSmoother(Derivs_Limiter(_velLimit, _accelLimit))
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
        smoothed = false;
        posDeltaSetpoint = 0;
        posDeltaSetpointTarget = 0;
        velSetpointTarget = 0;
        velSetpoint = 0;
        limitSetpointDistFromCurrent = false;
        timeFromSetpointLimit = max(_timeFromSetpointLimit, (float)0.0);
    }
    void run()
    {
        encoder.run();
        float time = (micros() - lastRunMicros) / 1000000.0;
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
                driverInRange = driver.set(setVal);
                posSetpoint = encoder.getPos();
            } else { //closed loop
                if (posMode) {
                    if (smoothed) { //setPosTarget
                        posSetpointSmoother.setPosition(encoder.getPos());
                        posSetpoint = posSetpointSmoother.calc();
                    } else { //setPosSetpoint
                        //nothing to do here, posSetpoint was set
                    }
                } else { //posDelta
                    if (posDeltaSetpoint != posDeltaSetpointTarget) {
                        if (posSetpointSmoother.getPreventGoingWrongWay() && posDeltaSetpointTarget != 0 && posDeltaSetpoint != 0 && ((posDeltaSetpointTarget > 0) != (posDeltaSetpoint > 0))) {
                            posDeltaSetpoint = 0;
                        }
                        posDeltaSetpointTarget = constrain(posDeltaSetpointTarget, -velLimit, velLimit);
                        posDeltaSetpointTarget = constrain(posDeltaSetpointTarget, -compensator.getMaxVel() * (getDriverMinRange() < 0), compensator.getMaxVel() * (getDriverMaxRange() > 0));

                        if (limitSetpointDistFromCurrent)
                            posDeltaSetpointTarget = constrain(posDeltaSetpointTarget, -timeFromSetpointLimit * getMaxVel(), timeFromSetpointLimit * getMaxVel());
                        posDeltaSetpoint += constrain(posDeltaSetpointTarget - posDeltaSetpoint, -accelLimit * time, accelLimit * time);
                    }

                    if (limitSetpointDistFromCurrent)
                        posDeltaSetpoint = constrain(posDeltaSetpoint, -timeFromSetpointLimit * getMaxVel(), timeFromSetpointLimit * getMaxVel());

                    posSetpoint += posDeltaSetpoint * time;

                    if (limitSetpointDistFromCurrent)
                        posSetpoint = constrain(posSetpoint, encoder.getPos() - timeFromSetpointLimit * getMaxVel(), encoder.getPos() + timeFromSetpointLimit * getMaxVel());
                }
                //MAKE MOTOR GO TO posSetpoint, by setting velSetpointTarget
                velSetpointTarget = constrain(posSetpoint - encoder.getPos(), -velLimit, velLimit);

                velSetpoint = velSetpointTarget;

                velSetpoint = constrain(velSetpoint, -compensator.getMaxVel() * (getDriverMinRange() < 0), compensator.getMaxVel() * (getDriverMaxRange() > 0));
                velSetpoint = constrain(velSetpoint, -velLimit, velLimit);
                setVal = compensator.compensate(velSetpoint);
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
        open = false;
        if (posMode == false) {
            posSetpointSmoother.resetTime();
            posSetpointSmoother.setVelocity(velSetpoint);
        }
        posMode = true;
        smoothed = true;
        posSetpointSmoother.setTarget(_posTarget);
        if (_run)
            run();
        return true; //did the target change?
    }
    bool setPosSetpoint(float _posSetpoint, bool _run = true)
    {
        open = false;
        posMode = true;
        smoothed = false;
        posSetpoint = _posSetpoint;
        if (_run)
            run();
        return true; //did the setpoint change?
    }
    bool setPosDelta(float _posDelta, bool _run = true, bool _resetPos = false)
    {
        open = false;
        posMode = false;
        limitSetpointDistFromCurrent = false;
        posDeltaSetpoint = _posDelta;
        posDeltaSetpointTarget = _posDelta;
        if (_resetPos)
            resetPos();
        if (_run)
            run();
        return true; //did the setting change?
    }
    void setAccelPosDelta(float _posDelta, bool _run = true, bool _resetPos = false)
    {
        if (open == true || posMode == true) {
            posDeltaSetpoint = velSetpoint;
        }
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

    float getPosTarget()
    {
        if (posMode && smoothed) {
            return posSetpointSmoother.getTarget();
        } else {
            return posSetpoint;
        }
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

    /**
     * @brief  returns how fast the motor driver is told to move (after compensator)
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
        if (_enable == true)
            lastRunMicros = micros();
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
        return driver.getMinRange();
    }

    float getDriverMaxRange()
    {
        return driver.getMaxRange();
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
