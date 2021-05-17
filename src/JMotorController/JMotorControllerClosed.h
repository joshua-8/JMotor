#ifndef J_MOTOR_CONTROLLER_CLOSED_H
#define J_MOTOR_CONTROLLER_CLOSED_H
#include "JEncoder/JEncoder.h"
#include "JMotorController.h"
#include "JMotorControllerBasic.h"
#include <Arduino.h>
class JMotorControllerClosed : public virtual JMotorController, public virtual JMotorControllerBase {
protected:
    JMotorDriver& driver;
    bool driverInRange;
    JMotorCompensator& compensator;
    Derivs_Limiter dL;

    float velLimit;
    float accelLimit;
    boolean open;
    float setVal;

public:
    JEncoder& encoder;
    //JControlLoop
    JMotorControllerClosed(JMotorDriver& _driver, JMotorCompensator& _compensator, JEncoder& _encoder, /*JControlLoop& _controllLoop,*/ float _velLimit = INFINITY, float _accelLimit = INFINITY)
        : driver(_driver)
        , compensator(_compensator)
        , dL(Derivs_Limiter(_velLimit, _accelLimit))
        , encoder(_encoder)
    {
        compensator.setDriverRange(driver.getMaxRange());
        velLimit = max(_velLimit, (float)0.0);
        accelLimit = max(_accelLimit, (float)0.0);
        open = true;
        setVal = 0;
    }
    void run()
    {
        
    }
    bool setVel(float vel, bool _run = true)
    {
        setPosDelta(constrain(vel, -getMaxVel(), getMaxVel()), _run, false);
    }
    bool setVelTarget(float vel, bool _run = true)
    {
        setAccelPosDelta(constrain(vel, -getMaxVel(), getMaxVel()), _run, false);
    }
    bool setPosTarget(float _posTarget)
    {
        open = false;
    }
    bool setPosSetpoint(float _posSetpoint)
    {
        open = false;
    }
    bool setPosDelta(float _posDelta, bool _run = true, bool _resetPos = true)
    {
        open = false;
    }
    void setAccelPosDelta(float _posDelta, bool _run = true, bool _resetPos = true)
    {
        open = false;
    }
    /**
     * @brief  
     * @note   
     * @param  vel: 
     * @retval 
     */
    bool setOpenVel(float vel)
    {
        open = true;
    }
    /**
     * @brief  
     * @note   
     * @param  vel: 
     * @retval 
     */
    bool setOpenVelTarget(float vel)
    {
        open = true;
    }

    float getVelTarget()
    {
    }
    float getVel()
    {
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

    void setAccelLimit(float _accelLimit)
    {
        accelLimit = max(_accelLimit, (float)0.0);
        dL.setAccelLimit(accelLimit);
    }
    void setVelLimit(float _velLimit)
    {
        velLimit = max(_velLimit, (float)0.0);
        dL.setVelLimit(velLimit);
    }

    float getDriverSetVal()
    {
        return setVal;
    }
    float getPosTarget()
    {
    }

    bool setEnable(bool _enable)
    {
        velocityOpen = 0;
        posDelta = 0;
        //reset
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
