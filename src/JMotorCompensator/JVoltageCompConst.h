#ifndef J_VOLTAGE_COMP_CONST_H
#define J_VOLTAGE_COMP_CONST_H
#include "JVoltageCompensator.h"
#include <Arduino.h>
/**
 * @brief  convert voltage to value needed to set driver at to get that voltage, with constant value for what supply voltage is
 */
class JVoltageCompConst : public JVoltageCompensator {
private:
    float supplyVoltage;

public:
    /**
     * @brief  constructor
     * @param  _supplyVoltage: (float) default: 1.0
     * @param  _driverRange: (float) what value makes the driver output 100% of supply voltage (default=1.0)
     */
    JVoltageCompConst(float _supplyVoltage = 1.0)
    {
        supplyVoltage = _supplyVoltage;
    }
    /**
     * @brief  do conversion
     * @param  voltage: (float) voltage wanted from motor driver output
     * @param  driverRange: (float) set what value makes the driver output 100% of supply voltage
     * @retval (float) value to set driver at
     */
    float adjust(float voltage, float driverRange)
    {
        return voltage / supplyVoltage * driverRange;
    }
    /**
     * @brief  adjust the setting of what the motor controller supply voltage is
     * @param  _supplyVoltage: (float) new setting for what the supply voltage is
     * @retval (bool) true if value changed
     */
    bool changeSupplyVoltageSetting(float _supplyVoltage)
    {
        if (supplyVoltage != _supplyVoltage) {
            supplyVoltage = _supplyVoltage;
            return true;
        }
        return false;
    }

    float getSupplyVoltage()
    {
        return supplyVoltage;
    }
};
#endif
