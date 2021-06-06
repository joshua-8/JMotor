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
     * @brief  set what pin the servo is connected to
     * @param  _pin: (byte)
     */
    virtual void setServoPin(byte _pin) = 0;

    /**
     * @brief  get what pin the servo is connected to
     * @retval (byte)
     */
    virtual byte getServoPin() = 0;
};
#endif
