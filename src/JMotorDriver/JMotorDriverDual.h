#ifndef J_MOTOR_DRIVER_DUAL_H
#define J_MOTOR_DRIVER_DUAL_H
#include "JMotorDriver.h"
#include <Arduino.h>
/**
 * @brief  control two JMotorDrivers (of any type) as if they are one driver
 * @note  one (or both) of the drivers may be another dual driver allowing for controlling more than two motors together
 */
class JMotorDriverDual : public JMotorDriver {
public:
    JMotorDriver& m1;
    JMotorDriver& m2;
    JMotorDriverDual(JMotorDriver& _m1, JMotorDriver& _m2)
        : m1(_m1)
        , m2(_m2)
    {
    }
    bool set(float val)
    {
        bool m1state = m1.set(val);
        bool m2state = m2.set(val);
        return m1state && m2state;
    }
    bool setEnable(bool _enable)
    {
        bool m1state = m1.setEnable(_enable);
        bool m2state = m2.setEnable(_enable);
        return m1state || m2state;
    }
    bool getEnable()
    {
        bool m1state = m1.getEnable();
        bool m2state = m2.getEnable();
        return m1state || m2state;
    }
    float getMaxRange()
    {
        return min(m1.getMaxRange(), m2.getMaxRange());
    }
    float getMinRange()
    {
        return max(m1.getMinRange(), m2.getMinRange());
    }
};
#endif
