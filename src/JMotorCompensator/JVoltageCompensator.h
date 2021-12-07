#ifndef J_VOLTAGE_COMPENSATOR_H
#define J_VOLTAGE_COMPENSATOR_H
#include <Arduino.h>
/**
 * @brief  convert voltage to value needed to set driver at to get that voltage
 */
class JVoltageCompensator {
public:
    /**
     * @brief  do conversion
     * @param  voltage: (float) voltage wanted from motor driver output
     * @param  driverRange: (float) set what value makes the driver output 100% of supply voltage
     * @retval (float) value to set driver at
     */
    virtual float adjust(float voltage, float driverRange);

    /**
     * @retval (float) returns current supply voltage
     */
    virtual float getSupplyVoltage();
};
#include "JVoltageCompAdjust.h"
#include "JVoltageCompConst.h"
#include "JVoltageCompMeasure.h"
#endif
