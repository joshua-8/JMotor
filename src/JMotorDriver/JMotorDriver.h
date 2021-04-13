#ifndef J_MOTOR_DRIVER_H
#define J_MOTOR_DRIVER_H
#include <Arduino.h>

/**
 * @brief  defines common interface for all types of JMotorDrivers
 * @note  JMotorDriver's functions are virtual, it can not be used directly
 */
class JMotorDriver {

public:
    /**
     * @brief  The JMotorDriverType enum is used so a specific subclass of JMotorDriver can return what type it is
     */
    enum JMotorDriverType {
        Null,
        avrServo,
        esp32Servo,
        avrPWM,
        esp32PWM,
        avrPWMDir,
        esp32PWMDir,
        avrL293,
        esp32L293,
    };

    /**
     * @brief  returns what type of driver it is
     * @retval (JMotorDriverType) 
     */
    virtual JMotorDriverType getType();

    /**
     * @brief  set motor power
     * @note  val should be between getMinRange and getMaxRange, but constrained internally
     * @param  val:  (float)
     * @retval true if at end of power range
     */
    virtual boolean set(float val);

    /**
     * @brief  use to enable or disable a motor, and sets up pin states
     * @note   setEnable(true) must be called before a motor driver will activate
     * @param  val: true=eneable, false=disable
     * @retval true if state changed, false if state already set
     */
    virtual boolean setEnable(boolean val);

    /**
     * @brief  get the enable state of the driver
     * @retval true if enabled, false if disabled
     */
    virtual boolean getEnable();
    
    /**
     * @brief  high end of the range
     * @note   usually 1.0
     * @retval (float)
     */
    virtual float getMaxRange();
    
    /**
     * @brief  low end of the range
     * @note   usually -1.0, if 0, that indicates a motor controller with no reverse function
     * @retval (float)
     */
    virtual float getMinRange();
};
#endif
