#ifndef J_MOTOR_CONTROLLER_BASIC_OPEN_H
#define J_MOTOR_CONTROLLER_BASIC_OPEN_H
#include "JMotorCompensator/JMotorCompensator.h"
#include "JMotorControllerBase.h"
#include "JMotorDriver/JMotorDriver.h"
#include <Arduino.h>

/**
 * @brief  A motor controller object that uses a JMotorCompensator to set velocity of a motor
 * @note  Open loop, and no position based commands
 */
class JMotorControllerBasic : public virtual JMotorControllerBase {
protected:
    float velocity;
    float setVal;
    float velocityTarget;
    float accelLimit;
    float velLimit;
    unsigned long lastRunMicros;

    JMotorDriver& driver;
    bool driverInRange;
    JMotorCompensator& compensator;

public:
    /**
     * @brief  constructor
     * @param  _driver: (JMotorDriver)
     * @param  _compensator: (JMotorCompensator)
     * @param  _accelLimit: max acceleration allowed for approaching velocityTarget, set to INFINITY for unlimited (default: INFINITY)
     */
    JMotorControllerBasic(JMotorDriver& _driver, JMotorCompensator& _compensator, float _velLimit = INFINITY, float _accelLimit = INFINITY)
        : driver(_driver)
        , compensator(_compensator)
    {
        compensator.setDriverRange(driver.getMaxRange());
        driverInRange = true;
        velocity = 0;
        accelLimit = max(_accelLimit, (float)0.0);
        velLimit = max(_velLimit, (float)0.0);
        velocityTarget = 0;
        setVal = 0;
        lastRunMicros = 0;
    }

    void setAccelLimit(float _accelLimit)
    {
        accelLimit = max(_accelLimit, (float)0.0);
    }

    void setVelLimit(float _velLimit)
    {
        velLimit = max(_velLimit, (float)0.0);
    }

    bool setVel(float vel, bool _run = true)
    {
        velocity = vel;
        velocityTarget = vel;
        if (_run)
            JMotorControllerBasic::run();
        return velocity == vel;
    }

    bool setVelTarget(float vel, bool _run = true)
    {
        velocityTarget = vel;
        if (_run)
            JMotorControllerBasic::run();
        return velocity == vel;
    }

    float getVelTarget()
    {
        return velocityTarget;
    }

    float getVel()
    {
        return velocity;
    }

    float getDriverSetVal()
    {
        return setVal;
    }

    void run()
    {
        if (getEnabled()) {
            if (velocity != velocityTarget) {
                velocity += constrain(velocityTarget - velocity, -accelLimit * (micros() - lastRunMicros) / 1000000.0, accelLimit * (micros() - lastRunMicros) / 1000000.0);
            }
            driverInRange = (abs(velocity) < compensator.getMaxVel());
            velocity = constrain(velocity, -compensator.getMaxVel() * (getDriverMinRange() < 0), compensator.getMaxVel() * (getDriverMaxRange() > 0));
            velocity = constrain(velocity, -velLimit, velLimit);
            lastRunMicros = micros();
            if (driver.getEnable()) {
                float lastSetVal = setVal;
                setVal = compensator.compensate(velocity);
                if (setVal != lastSetVal) {
                    driver.set(setVal);
                }
            }
        }
    }

    bool setEnable(bool _enable)
    {
        if (!_enable && getEnabled()) {
            velocity = 0;
            velocityTarget = 0;
        }
        lastRunMicros = micros();
        return driver.setEnable(_enable);
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

    bool getEnabled()
    {
        return driver.getEnable();
    }

    float getMaxVel()
    {
        return min(velLimit, compensator.getMaxVel());
    }

    float getMinVel()
    {
        return min(velLimit, compensator.getMinVel());
    }

    void setMaxDriverRangeAmount(float _driverRangeAmount)
    {
        compensator.setMaxDriverRangeAmount(_driverRangeAmount);
    }
};
#endif
