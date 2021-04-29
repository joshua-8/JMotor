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
        return val * multiplier;
    }
    /**
     * @param  _multiplier: (float) driver units per speed units
     * @retval (boolean) true if multiplier changed
     */
    boolean setMultiplier(float _multiplier)
    {
        if (multiplier != _multiplier) {
            multiplier = _multiplier;
            return true;
        }
        return false;
    }
};
#endif