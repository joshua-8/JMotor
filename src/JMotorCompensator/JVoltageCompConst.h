#ifndef J_VOLTAGE_COMP_CONST_H
#define J_VOLTAGE_COMP_CONST_H
#include "JVoltageCompensator.h"
#include <Arduino.h>
class JVoltageCompConst : public JVoltageCompensator {
private:
    float supplyVoltage;

public:
    JVoltageCompConst(float _supplyVoltage, float _driverRange = 1.0)
        : JVoltageCompensator(_driverRange)
        , supplyVoltage(_supplyVoltage)
    {
    }
    float adjust(float voltage)
    {
        return voltage / supplyVoltage * driverRange;
    }
    boolean changeSupplyVoltageSetting(float _supplyVoltage)
    {
        if (supplyVoltage != _supplyVoltage) {
            supplyVoltage = _supplyVoltage;
            return true;
        }
        return false;
    }
};
#endif
