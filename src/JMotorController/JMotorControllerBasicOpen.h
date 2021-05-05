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
class JMotorControllerBasicOpen : public JMotorControllerBase {
protected:
    float velocity;
    float setVal;
    float velocityTarget;
    float accelLimit;
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
    JMotorControllerBasicOpen(JMotorDriver& _driver, JMotorCompensator& _compensator, float _accelLimit = INFINITY)
        : driver(_driver)
        , compensator(_compensator)
    {
        compensator.setDriverRange(driver.getMaxRange());
        driverInRange = true;
        velocity = 0;
        accelLimit = _accelLimit;
        velocityTarget = 0;
        setVal = 0;
        lastRunMicros = 0;
    }

    /**
     * @brief  set maximum rate that motor speed can be changed at
     * @note   set to INFINITY to disable acceleration limiting
     * @param  _accelLimit: (float)
     */
    void setAccelLimit(float _accelLimit)
    {
        accelLimit = max(_accelLimit, (float)0.0);
    }

    void setVel(float vel)
    {
        velocity = vel;
        velocityTarget = vel;
        run();
    }

    float setVelTarget(float vel)
    {
        velocityTarget = vel;
        run();
        return velocity;
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
        if (velocity != velocityTarget) {
            velocity += constrain(velocityTarget - velocity, -accelLimit * (micros() - lastRunMicros) / 1000000.0, accelLimit * (micros() - lastRunMicros) / 1000000.0);

            velocity = constrain(velocity, -compensator.getMaxVel(), compensator.getMaxVel());
        }
        lastRunMicros = micros();
        if (driver.getEnable()) {
            float lastSetVal = setVal;
            setVal = compensator.compensate(velocity);
            if (setVal != lastSetVal) {
                driverInRange = driver.set(setVal);
            }
        }
    }

    bool setEnable(bool _enable)
    {
        velocity = 0;
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
        return compensator.getMaxVel();
    }
};
#endif
