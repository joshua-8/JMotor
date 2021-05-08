#ifndef J_VOLTAGE_COMP_ADJUST_H
#define J_VOLTAGE_COMP_ADJUST_H
#include "JVoltageCompensator.h"
#include <Arduino.h>
/**
 * @brief  convert voltage to value needed to set driver at to get that voltage, uses reference to variable that should be equal to measured supply voltage
 */
class JVoltageCompAdjust : public JVoltageCompensator {
private:
    float& supplyVoltage;

public:
    /**
     * @brief  constructor
     * @param  _supplyVoltage: (float) reference to a variable that contains the current voltage supplied to the motor controller
     * @param  _driverRange: (float) what value makes the driver output 100% of supply voltage (default=1.0)
     * @retval 
     */
    JVoltageCompAdjust(float& _supplyVoltage)
        : supplyVoltage(_supplyVoltage)
    {
    }
    float adjust(float voltage, float driverRange)
    {
        return voltage / supplyVoltage * driverRange;
    }
    /**
     * @brief  if for some reason you want to provide a new reference to a variable that contains the current voltage supplied to the motor controller
     * @param  _supplyVoltage: (float)
     */
    void changeSupplyVoltageVariable(float& _supplyVoltage)
    {
        supplyVoltage = _supplyVoltage;
    }

    float getSupplyVoltage()
    {
        return supplyVoltage;
    }
};
#endif
