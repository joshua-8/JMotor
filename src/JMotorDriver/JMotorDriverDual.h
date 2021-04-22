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
    boolean set(float val)
    {
        boolean m1state = m1.set(val);
        boolean m2state = m2.set(val);
        return m1state && m2state;
    }
    boolean setEnable(boolean val)
    {
        boolean m1state = m1.setEnable(val);
        boolean m2state = m2.setEnable(val);
        return m1state || m2state;
    }
    boolean getEnable()
    {
        boolean m1state = m1.getEnable();
        boolean m2state = m2.getEnable();
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
