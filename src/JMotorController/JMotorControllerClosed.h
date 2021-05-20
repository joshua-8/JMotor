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
    Derivs_Limiter posSetpointSmoother;

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

public:
    JEncoder& encoder;
    //JControlLoop
    JMotorControllerClosed(JMotorDriver& _driver, JMotorCompensator& _compensator, JEncoder& _encoder, /*JControlLoop& _controllLoop,*/ float _velLimit = INFINITY, float _accelLimit = INFINITY)
        : driver(_driver)
        , compensator(_compensator)
        , posSetpointSmoother(Derivs_Limiter(_velLimit, _accelLimit))
        , encoder(_encoder)
    {
        compensator.setDriverRange(driver.getMaxRange());
        velLimit = max(_velLimit, (float)0.0);
        accelLimit = max(_accelLimit, (float)0.0);
        open = true;
        setVal = 0;
        lastRunMicros = 0;
        posMode = false;
    }
    void run()
    {
        encoder.run();
        float time = (micros() - lastRunMicros) / 1000000.0;
        lastRunMicros = micros();
        if (getEnable()) {
            if (open) {
                if (velSetpoint != velSetpointTarget) {
                    velSetpoint += constrain(velSetpointTarget - velSetpoint, -accelLimit * time, accelLimit * time);
                }
                velSetpoint = constrain(velSetpoint, -compensator.getMaxVel() * (getDriverMinRange() < 0), compensator.getMaxVel() * (getDriverMaxRange() > 0));
                velSetpoint = constrain(velSetpoint, -velLimit, velLimit);
                setVal = compensator.compensate(velSetpoint);
                driverInRange = driver.set(setVal);
            } else { //closed loop
            }
            lastRunMicros = micros();
        } //enabled
    }

    void setVel(float vel, bool _run = true)
    {
        setPosDelta(constrain(vel, -getMaxVel(), getMaxVel()), _run, false);
    }
    void setVelTarget(float vel, bool _run = true)
    {
        setAccelPosDelta(constrain(vel, -getMaxVel(), getMaxVel()), _run, false);
    }
    bool setPosTarget(float _posTarget, bool _run = true)
    {
        open = false;
        if (posMode == false) {
            posSetpointSmoother.resetTime();
        }
        posMode = true;
        posSetpointSmoother.setTarget(_posTarget);
        return true; //did the target change?
    }
    bool setPosSetpoint(float _posSetpoint, bool _run = true)
    {
        open = false;
        posMode = false;
        velSetpoint = 0;
        velSetpointTarget = 0;
        posSetpoint = _posSetpoint;
        return true; //did the setpoint change?
    }
    bool setPosDelta(float _posDelta, bool _resetPos = true, bool _run = true)
    {
        open = false;
        posMode = false;
        velSetpointTarget = _posDelta;
        velSetpoint = _posDelta;
        if (_resetPos)
            resetPos();
        return true; //did the setting change?
    }
    void setAccelPosDelta(float _posDelta, bool _resetPos = true, bool _run = true)
    {
        open = false;
        posMode = false;
        velSetpointTarget = _posDelta;
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
        return 0; // todo
    }

    float getVelTarget()
    {
        return velSetpointTarget;
    }
    /**
     * @brief  returns how fast the motor should be turning
     * @note   may be acceleration limited, use getVel for actual encoder reading
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
        return encoder.getDist();
    }
    float resetPos()
    {
        float oldDist = encoder.zeroCounter() * encoder.getDistPerCountFactor();
        //reset targets
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
