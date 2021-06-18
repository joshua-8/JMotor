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
     * @retval (bool) false if at end of power range, true otherwise
     */
    virtual bool set(float val);

    /**
     * @brief  use to enable or disable a motor, and sets up pin states
     * @note   setEnable(true) must be called before a motor driver will activate
     * @param  _enable: (bool) true=enable, false=disable
     * @retval (bool) true if state changed, false if state already set
     */
    virtual bool setEnable(bool _enable);

    /**
     * @brief  get the enable state of the driver
     * @retval (bool) true if enabled, false if disabled
     */
    virtual bool getEnable();

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

    /**
     * @brief  enable motor
     * @note equivalent to setEnable(true)
     * @retval (bool) true if state changed, false if state already set
     */
    bool enable()
    {
        return setEnable(true);
    }

    /**
     * @brief  disable motor
     * @note equivalent to setEnable(false)
     * @retval (bool) true if state changed, false if state already set
     */
    bool disable()
    {
        return setEnable(false);
    }
};
#if defined(ESP32)
#include "JMotorDriverEsp32L293.h"
#include "JMotorDriverEsp32PWM.h"
#include "JMotorDriverEsp32PWMDir.h"
#include "JMotorDriverEsp32Servo.h"
#endif
#if !defined(ESP32) || defined(J_MOTOR_DRIVER_FORCE_ANALOGWRITE) || defined(_ESP32_ESP32S2_ANALOG_WRITE_)
#include "JMotorDriverAvrL293.h"
#include "JMotorDriverAvrPWM.h"
#include "JMotorDriverAvrPWMDir.h"
#endif
#if !defined(ESP32)
#include "JMotorDriverAvrServo.h"
#endif
#include "JMotorDriverDual.h"
#include "JMotorDriverServo.h"
#include "JMotorDriverServoAdvancedDual.h"
#include "JMotorDriverServoDual.h"

#endif
