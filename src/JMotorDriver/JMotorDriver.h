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
     * @param  _enable: (boolean) true=enable, false=disable
     * @retval (boolean) true if state changed, false if state already set
     */
    virtual boolean setEnable(boolean _enable);

    /**
     * @brief  enable motor
     * @note equivalent to setEnable(true)
     * @retval (boolean) true if state changed, false if state already set
     */
    boolean enable()
    {
        return setEnable(true);
    }

    /**
     * @brief  disable motor
     * @note equivalent to setEnable(false)
     * @retval (boolean) true if state changed, false if state already set
     */
    boolean disable()
    {
        return setEnable(false);
    }

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
/**
 * @brief  use #define JMOTOR_DRIVER_FORCE_ANALOGWRITE to override automatic board detection and make the standard analogWrite versions load (use if you're using an esp32 and this library https://github.com/ERROPiX/ESP32_AnalogWrite)
 */
#if defined(ESP32) && !defined(JMOTOR_DRIVER_FORCE_ANALOGWRITE) // ESP32s do pwm differently
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
#include "JMotorDriverDual.h"

#endif
