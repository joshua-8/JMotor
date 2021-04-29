#ifndef J_VOLTAGE_COMPENSATOR_H
#define J_VOLTAGE_COMPENSATOR_H
#include <Arduino.h>
/**
 * @brief  convert voltage to value needed to set driver at to get that voltage
 */
class JVoltageCompensator {
protected:
    float driverRange = 1.0;

    JVoltageCompensator(float _driverRange)
        : driverRange(_driverRange)
    {
    }

public:
    /**
     * @brief  do conversion
     * @param  voltage: (float) voltage wanted from motor driver output
     * @retval (float) value to set driver at
     */
    virtual float adjust(float voltage);

    /**
     * @param  _driverRange: (float) driver units per speed units
     * @retval (boolean) true if multiplier changed
     */
    boolean setDriverRange(float _driverRange)
    {
        if (driverRange != _driverRange) {
            driverRange = _driverRange;
            return true;
        }
        return false;
    }
};
#include "JVoltageCompAdjust.h"
#include "JVoltageCompConst.h"
#endif