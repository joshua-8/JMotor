#ifndef J_MOTOR_CONTROLLER_BASIC_OPEN_H
#define J_MOTOR_CONTROLLER_BASIC_OPEN_H
#include "JMotorControllerBase.h"
#include <Arduino.h>
/**
 * @brief  A motor controller object that uses a JMotorCompensator to 
 * @note   
 * @retval None
 */
class JMotorControllerBasicOpen : public JMotorControllerBase {
private:
    float velocity;
    float setVal;

public:
    JMotorControllerBasicOpen(JMotorDriver& _driver, JMotorCompensator& _compensator)
        : JMotorControllerBase(_driver, _compensator)
    {
        velocity = 0;
    }

    void setVel(float vel)
    {
        velocity = vel;
        run();
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
        if (driver.getEnable()) {
            float lastSetVal = setVal;
            setVal = compensator.compensate(velocity);
            if (setVal != lastSetVal) {
                driver.set(setVal);
            }
        }
    }

    boolean setEnable(boolean _enable)
    {
        return driver.setEnable(_enable);
    }
};
#endif
