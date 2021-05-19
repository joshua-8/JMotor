#ifndef J_MOTOR_DRIVER_SERVO_ADVANCED_DUAL_H
#define J_MOTOR_DRIVER_SERVO_ADVANCED_DUAL_H
#include "JMotorDriverServoAdvanced.h"
#include <Arduino.h>
/**
 * @brief  control two JMotorDriverServoAdvanced as if they are one driver
 * @note  one (or both) of the drivers may be another dual driver allowing for controlling more than two servos together
 */
class JMotorDriverServoAdvancedDual : public JMotorDriverServoAdvanced {
public:
    JMotorDriverServoAdvanced& m1;
    JMotorDriverServoAdvanced& m2;
    JMotorDriverServoAdvancedDual(JMotorDriverServoAdvanced& _m1, JMotorDriverServoAdvanced& _m2)
        : m1(_m1)
        , m2(_m2)
    {
    }
    void adjustFrequency(float freq)
    {
        m1.adjustFrequency(freq);
        m2.adjustFrequency(freq);
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

    void setMinServoValue(int value)
    {
        m1.setMinServoValue(value);
        m2.setMinServoValue(value);
    }
    void setMaxServoValue(int value)
    {
        m1.setMaxServoValue(value);
        m2.setMaxServoValue(value);
    }
    int getMinServoValue()
    {
        return min(m1.getMinServoValue(), m2.getMinServoValue());
    }
    int getMaxServoValue()
    {
        return max(m1.getMaxServoValue(), m2.getMaxServoValue());
    }
    int getServoValueRange()
    {
        return getMaxServoValue() - getMinServoValue();
    }
    void setServoValues(int min, int max)
    {
        setMinServoValue(min);
        setMaxServoValue(max);
    }
};
#endif
