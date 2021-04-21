
#ifndef J_MOTOR_DRIVER_H
#define J_MOTOR_DRIVER_H
#include <Arduino.h>

//**********SEE BOTTOM OF THIS FILE FOR #INCLUDES OF SUBCLASSES**********//
/**
 * @brief  defines common interface for all types of JMotorDrivers
 * @note  JMotorDriver's functions are virtual, it can not be used directly
 */
class JMotorDriver {
public:
    /**
     * @brief  set motor power
     * @note  val should be between getMinRange and getMaxRange, but constrained internally
     * @param  val:  (float) val
     * @retval (boolean) false if at end of power range, true otherwise
     */
    virtual boolean set(float val);

    /**
     * @brief  use to enable or disable a motor, and sets up pin states
     * @note   setEnable(true) must be called before a motor driver will activate
     * @param  val: (boolean) true=eneable, false=disable
     * @retval (boolean) true if state changed, false if state already set
     */
    virtual boolean setEnable(boolean val);

    /**
     * @brief  get the enable state of the driver
     * @retval (boolean) true if enabled, false if disabled
     */
    virtual boolean getEnable();

    /**
     * @brief  high end of the range
     * @note   usually 1.0
     * @retval (float) maxRange
     */
    virtual float getMaxRange();

    /**
     * @brief  low end of the range
     * @note   usually -1.0, if 0, that indicates a motor controller with no reverse function
     * @retval (float) minRange
     */
    virtual float getMinRange();
};

#if defined(ESP32) // ESP32s do pwm differently
#include "JMotorDriverEsp32L293.h"
#include "JMotorDriverEsp32PWM.h"
#include "JMotorDriverEsp32PWMDir.h"
#include "JMotorDriverEsp32Servo.h"
#else // "standard" Arduino commands (analogWrite())
#include "JMotorDriverAvrL293.h"
#include "JMotorDriverAvrPWM.h"
#include "JMotorDriverAvrPWMDir.h"
#include "JMotorDriverAvrServo.h"
#endif

#endif
