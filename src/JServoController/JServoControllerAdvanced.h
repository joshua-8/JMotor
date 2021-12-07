#ifndef J_SERVO_CONTROLLER_ADVANCED_H
#define J_SERVO_CONTROLLER_ADVANCED_H
#include "JMotorDriver/JMotorDriverServoAdvanced.h"
#include "JServoController.h"
#include <Arduino.h>
/**
 * @brief  class for controlling JMotorDriverServoAdvanced, with power reducing option along with standard JServoController features
 * @note this servo controller supports adjusting the servo signal frequency which for many servos can change how hard the servos push
 */
class JServoControllerAdvanced : public JServoController {
protected:
    /**
     * @brief  reference to the servo motor driver
     * @note   this reference is of the advanced type so can set frequency.
     * @retval None
     */
    JMotorDriverServoAdvanced& adServo;
    unsigned long weakenTimeout;
    float defaultFreq;
    float weakFreq;
    bool weakened;
    unsigned long startWeakTimeout;
    unsigned long enabledMillis;

public:
    /**
     * @brief  Constructor for JServoControllerAdvanced, a class for controlling JMotorDriverServoAdvanced, with angle calibration and accel and velocity limiting
     * @note  the "advanced" servo driver supports adjusting the servo signal frequency
     * @param  _servo: (JMotorDriverServoAdvanced&) reference to an instance of a class that's a subclass of JMotorDriverServo
     * @param  _weakFreq: (float) default: .5, what fraction of normal frequency the signal should go to in order to weaken the servo's strength
     * @param  _weakenTimeout: (unsigned long) default: 0, after how many milliseconds of no movement should the servo be set to lower power? 0=never switch to low power
     * @param  _normalFreq: (float) default: 1, what fraction of 50Hz should be used
     * @param  _startWeakTimeout: (unsigned long) default 0, how many milliseconds after enabling should the servo be in weakened mode (to soften jumps at startup)? 0=inactive
     * @param  _reverse: (bool) default: false, use to reverse direction of servo
     * @param  velLimit: (float) default: INFINITY, maximum velocity you want the servo to move at in limited mode
     * @param  accelLimit: (float) default: INFINITY, maximum acceleration you want the servo to move at in limited mode
     * @param  _disableTimeout: (unsigned long) default: 0, after how many milliseconds of no movement should the servo be disabled? 0=never disable
     * @param  _minAngleLimit: (float) minimum angle limit for servo
     * @param  _maxAngleLimit: (float) maximum angle limit for servo
     * @param  _pos: (float) default: 90, value to set servo position to at startup
     * @param  _minSetAngle: (float) default: 0, when servo driver is set to its minimum (probably -1) what angle does the servo go to?
     * @param  _maxSetAngle: (float) default: 180, when servo driver is set to its maximum (probably 1) what angle does the servo go to?
     * @param  minServoVal: (int) default: 544, microseconds for servo signal pulse for minimum angle
     * @param  maxServoVal: (int) default: 2400, microseconds for servo signal pulse for maximum angle
     * @param  _preventGoingWrongWay (bool) default: true, immediately stop if going away from target
     * @param  _preventGoingTooFast (bool) default: true, immediately slow down if set to a speed above velLimit
     * @param  _stoppingAccelLimit (float) default: INFINITY, 

     */
    JServoControllerAdvanced(JMotorDriverServoAdvanced& _servo, float _weakFreq = .5, unsigned long _weakenTimeout = 0, float _normalFreq = 1.0, unsigned long _startWeakTimeout = 0, bool _reverse = false, float velLimit = INFINITY, float accelLimit = INFINITY, unsigned long _disableTimeout = 0, float _minAngleLimit = 0, float _maxAngleLimit = 180, float _pos = 90, float _minSetAngle = 0, float _maxSetAngle = 180, int minServoVal = 544, int maxServoVal = 2400, bool _preventGoingWrongWay = true, bool _preventGoingTooFast = true, float _stoppingAccelLimit = INFINITY)
        : JServoController(_servo, _reverse, velLimit, accelLimit, _disableTimeout, _minAngleLimit, _maxAngleLimit, _pos, _minSetAngle, _maxSetAngle, minServoVal, maxServoVal, _preventGoingWrongWay, _preventGoingTooFast, _stoppingAccelLimit)
        , adServo(_servo)
    {
        weakenTimeout = _weakenTimeout;
        startWeakTimeout = _startWeakTimeout;
        defaultFreq = _normalFreq;
        weakFreq = _weakFreq;
        adServo.adjustFrequency(_normalFreq);
        weakened = false;
        enabledMillis = millis();
    }
    virtual void run()
    {
        if (startWeakTimeout != 0 && millis() - enabledMillis < startWeakTimeout) {
            if (!weakened) {
                setStrengthWeak();
            }
            weakened = true;
        } else if (weakenTimeout != 0) {
            if (millis() - lastMovedMillis > weakenTimeout) {
                if (weakened == false) {
                    setStrengthWeak();
                }
                weakened = true;
            } else {
                if (weakened == true) {
                    setStrengthNormal();
                }
                weakened = false;
            }
        }
        JServoController::run();
    }
    //override JServoController
    bool setEnable(bool _enable)
    {
        enabledMillis = millis();
        return JServoController::setEnable(_enable);
    }
    /**
     * @brief  set servo to weaker setting
     */
    void setStrengthWeak()
    {
        weakened = true;
        adjustServoFrequency(weakFreq);
    }
    /**
     * @brief  set servo to normal setting
     */
    void setStrengthNormal()
    {
        weakened = false;
        adjustServoFrequency(defaultFreq);
    }
    /**
     * @brief  adjust frequency as a fraction of standard 50hz frequency
     * lower frequencies (try .75) may let you reduce how strongly the servo holds its position
     * @note   different servos may respond differently to non-normal signals, including possibly being damaged, so use at your own risk!
     * @param  freq: (float) freq*50=setFrequency
     */
    void adjustServoFrequency(float freq = 1.0)
    {
        adServo.adjustFrequency(freq);
        writeAngleToServo(dL.getPosition());
    }
    /**
     * @brief  change setting for what fraction of normal frequency the signal should go to in order to weaken the servo's strength
     * @param  _weak: (float)
     */
    void setWeakFreq(float _weak)
    {
        weakFreq = _weak;
        if (weakened) {
            adjustServoFrequency(weakFreq);
        }
    }
    void setNormalFreq(float _norm)
    {
        defaultFreq = _norm;
        if (!weakened) {
            adjustServoFrequency(defaultFreq);
        }
    }
    bool getWeakened()
    {
        return weakened;
    }
    /**
     * @note   if set to zero (disabling weakening after a timeout) strength remains at the last set value until you use setStrengthWeak() or setStrengthNormal()
     */
    void setWeakTimeout(unsigned long _timeout)
    {
        weakenTimeout = _timeout;
    }
    unsigned long getWeakenTimeout()
    {
        return weakenTimeout;
    }
    void setStartWeakTimeout(unsigned long _timeout)
    {
        startWeakTimeout = _timeout;
    }
    unsigned long getStartWeakTimeout()
    {
        return startWeakTimeout;
    }
    float getWeakFreq()
    {
        return weakFreq;
    }
    float getNormalFreq()
    {
        return defaultFreq;
    }
};
#endif