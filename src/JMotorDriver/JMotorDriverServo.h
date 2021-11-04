#ifndef J_MOTOR_DRIVER_SERVO_H
#define J_MOTOR_DRIVER_SERVO_H
#include "JMotorDriver.h"
#include <Arduino.h>
/**
 * @brief  class that can be used to specify a driver that controls specifically a servo
 */
class JMotorDriverServo : public JMotorDriver {
protected:
    int minServoValue;
    int maxServoValue;
    /**
     * how many microseconds the servo signal pulse was most recently set to
     */
    int setMicroseconds = 0;
    bool constrainRange;

public:
    /**
     * @brief  microseconds for shortest servo pulse
     * @param  value: (int)
     */
    void setMinServoValue(int value)
    {
        minServoValue = value;
    }
    /**
     * @brief  microseconds for longest servo pulse
     * @param  value: (int)
     */
    void setMaxServoValue(int value)
    {
        maxServoValue = value;
    }
    /**
     * @brief  returns setting for microseconds for shortest servo pulse
     * @retval  (int)
     */
    int getMinServoValue()
    {
        return minServoValue;
    }
    /**
     * @brief  returns setting for microseconds for longest servo pulse
     * @retval  (int)
     */
    int getMaxServoValue()
    {
        return maxServoValue;
    }
    /**
     * @brief  returns difference (in microseconds) between longest and shortest servo pulse settings
     * @retval  (int)
     */
    int getServoValueRange()
    {
        return maxServoValue - minServoValue;
    }
    /**
     * @brief  set the settings for short and long servo pulses at the same time
     * @param  min: (int) microseconds, often 544
     * @param  max: (int) microseconds, often 2400
     */
    void setServoValues(int min, int max)
    {
        setMinServoValue(min);
        setMaxServoValue(max);
    }

    /**
     * @brief  returns how many microseconds the servo signal pulse was most recently set to
     * @retval (int)
     */
    int getSetMicroseconds()
    {
        return setMicroseconds;
    }
    /**
     * @brief  returns whether the range of set() will be constrained to within -1 and 1
     * @retval (bool)
     */
    bool getConstrainRange()
    {
        return constrainRange;
    }
    /**
     * @brief  set whether the range of set() will be constrained to within -1 and 1
     * @param  _constrainRange: (bool)
     */
    void setConstrainRange(bool _constrainRange)
    {
        constrainRange = _constrainRange;
    }
};
#endif
