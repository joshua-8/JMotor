#ifndef J_MOTOR_COMP_DIRECT_H
#define J_MOTOR_COMP_DIRECT_H
#include "JMotorCompensator.h"
#include <Arduino.h>
/**
 * @brief  Converts from speed to driver input with a simple multiplier.
 */
class JMotorCompDirect : public JMotorCompensator {
private:
    float multiplier;

public:
    /**
     * @brief  constructor
     * @param  _multiplier: (float) driver units per speed units
     */
    JMotorCompDirect(float _multiplier)
    {
        multiplier = _multiplier;
    }
    float compensate(float val)
    {
        return constrain(val * multiplier, -maxDriverRange, maxDriverRange);
    }
    /**
     * @param  _multiplier: (float) driver units per speed units
     * @retval (bool) true if multiplier changed
     */
    bool setMultiplier(float _multiplier)
    {
        if (multiplier != _multiplier) {
            multiplier = _multiplier;
            return true;
        }
        return false;
    }
    float getMaxVel()
    {
        return driverRange / multiplier * maxDriverRange;
    }

    float getMinVel()
    {
        return 0;
    }
};
#endif