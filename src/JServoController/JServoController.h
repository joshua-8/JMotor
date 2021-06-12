#ifndef J_SERVO_CONTROLLER_H
#define J_SERVO_CONTROLLER_H
#include "Derivs_Limiter.h"
#include "JMotorDriver/JMotorDriverServo.h"
#include <Arduino.h>
//see bottom of file for #includes of subclasses and other JServoController relevant files
/**
 * @brief  class for controlling JMotorDriverServo, with angle calibration and accel and velocity limiting
 * @note  after creating a ServoController object with a ServoDriver, using the Driver's methods directly is not recommended
 */
class JServoController {
protected:
    /**
     * @brief  reference to driver that's a subclass of JMotorDriverServo
     */
    JMotorDriverServo& servo;
    /**
     * @brief  variable for how long to wait if not moved before disabling (default: 0, never disable)
     */
    unsigned long disableTimeout;
    /**
     * @brief  lower limit for angle setpoint
     * @note (if greater than maxAngleLimit, direction is reversed)
     */
    float minAngleLimit;
    /**
     * @brief  higher limit for angle setpoint
     * @note (if less than minAngleLimit, direction is reversed)
     */
    float maxAngleLimit;
    /**
     * @brief  when servodriver is set to getMinValue what angle is the servo?
     */
    float minSetAngle;
    /**
     * @brief  when servodriver is set to getMaxValue what angle is the servo?
     */
    float maxSetAngle;
    /**
     * @brief  reverse range of servo
     */
    bool reverse;
    /**
     * @brief  variable used to keep track of how long the servo has been still for
     */
    unsigned long lastMovedMillis;
    /**
     * @brief  keep track of whether controller is enabled (since evne when controller is enabled driver might be disabled if timedout and sleeping)
     */
    bool enabled;
    /**
     * @brief  has it been longer than disableTimeout since the servo has moved?
     */
    bool sleeping;
    /**
     * @brief  used for telling if the driver needs to be set to a new value
     */
    bool rewriteToServo;

public:
    /**
     * @brief  instance of Derivs_Limiter class used to smoothly move servo
     * @note   https://github.com/joshua-8/Derivs_Limiter 
     */
    Derivs_Limiter dL;

    /**
     * @brief  Constructor for JServoController, a class for controlling JMotorDriverServo, with angle calibration and accel and velocity limiting
     * @param  _servo: (JMotorDriverServo&) reference to an instance of a class that's a subclass of JMotorDriverServo
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
     * */
    JServoController(JMotorDriverServo& _servo, bool _reverse = false, float velLimit = INFINITY, float accelLimit = INFINITY, unsigned long _disableTimeout = 0, float _minAngleLimit = 0, float _maxAngleLimit = 180, float _pos = 90, float _minSetAngle = 0, float _maxSetAngle = 180, int minServoVal = 544, int maxServoVal = 2400, bool _preventGoingWrongWay = true, bool _preventGoingTooFast = true, float _stoppingAccelLimit = INFINITY)
        : servo(_servo)
        , dL(Derivs_Limiter(max(velLimit, float(0.0)), max(accelLimit, float(0.0)), _pos, _pos, 0, _preventGoingWrongWay, _preventGoingTooFast, min(_minAngleLimit, _maxAngleLimit), max(_minAngleLimit, _maxAngleLimit), _stoppingAccelLimit))
    {
        enabled = false;
        sleeping = false;
        lastMovedMillis = 0;
        minAngleLimit = _minAngleLimit;
        maxAngleLimit = _maxAngleLimit;
        disableTimeout = _disableTimeout;
        reverse = _reverse;
        minSetAngle = _minSetAngle;
        maxSetAngle = _maxSetAngle;
        servo.setServoValues(minServoVal, maxServoVal);
        rewriteToServo = true;
    }
    /**
     * @brief  call this in your main loop
     */
    virtual void run()
    {
        if (enabled)
            dL.calc();
        if (dL.getPositionDelta() != 0.0) {
            lastMovedMillis = millis();
        }
        if (disableTimeout != 0) {
            sleeping = millis() - lastMovedMillis > disableTimeout;
            if (sleeping) {
                if (servo.getEnable() == true) {
                    servo.disable();
                }
            } else { //activate
                if (servo.getEnable() == false && enabled) {
                    servo.enable();
                    rewriteToServo = true;
                }
            }
        }
        if (dL.getPositionDelta() != 0.0) {
            rewriteToServo = true;
        }
        if (servo.getEnable() && enabled) {
            writeAngleToServo(dL.getPosition());
        }
    }
    /**
     * @brief  set servo angle immediately, without velocity or acceleration limiting
     * @param  angle: (float) angle to set
     * @param  _run: (bool) default: true, true = call run() in this function, false = you'll call run() yourself
     */
    void setAngleImmediate(float angle, bool _run = true)
    {
        angle = constrain(angle, min(minAngleLimit, maxAngleLimit), max(minAngleLimit, maxAngleLimit));
        if (dL.getTarget() != angle) {
            wake();
        }
        dL.setTarget(angle);
        dL.setPosition(angle);
        if (_run)
            run();
    }
    /**
     * @brief  set servo angle target, servo will move to target but at limited velocity and acceleration
     * @param  angle: (float) angle to set
     * @param  _run: (bool) default: true, true = call run() in this function, false = you'll call run() yourself
     */
    void setAngleSmoothed(float angle, bool _run = true)
    {
        angle = constrain(angle, min(minAngleLimit, maxAngleLimit), max(minAngleLimit, maxAngleLimit));
        if (dL.getTarget() != angle)
            wake();
        dL.setTarget(angle);
        if (_run)
            run();
    }
    void setDisableTimeout(unsigned long _timeout)
    {
        disableTimeout = _timeout;
    }
    unsigned long getDisableTimeout()
    {
        return disableTimeout;
    }
    unsigned long getLastMovedMillis()
    {
        return lastMovedMillis;
    }
    unsigned long getMillisSinceMoved()
    {
        return millis() - lastMovedMillis;
    }
    void setLastMovedMillis(unsigned long mil)
    {
        lastMovedMillis = mil;
    }
    virtual bool setEnable(bool _enable)
    {

        if (_enable && !enabled) {
            wake();
            restartRun();
            dL.setVelocity(0);
        }
        servo.setEnable(_enable);
        if (enabled != _enable) {
            enabled = _enable;
            return true;
        } else
            return false;
    }
    bool enable()
    {
        return setEnable(true);
    }
    bool disable()
    {
        return setEnable(false);
    }
    bool getEnable()
    {
        return enabled;
    }
    float getPosTarget()
    {
        return dL.getTarget();
    }
    float getPos()
    {
        return dL.getPosition();
    }
    float getVelocity()
    {
        return dL.getVelocity();
    }
    bool isPosAtTarget()
    {
        return dL.isPosAtTarget();
    }
    bool isPosNotAtTarget()
    {
        return dL.isPosNotAtTarget();
    }
    float distTotarget()
    {
        return dL.distToTarget();
    }
    /**
     * @brief  if you stopped calling run() for a while, call this before restarting run() to avoid a big jump in movement
     */
    void restartRun()
    {
        dL.resetTime();
    }
    bool getActive()
    {
        return !sleeping;
    }
    void wake()
    {
        rewriteToServo = true;
        lastMovedMillis = millis();
    }
    void setReverse(bool rev)
    {
        if (rev != reverse) {
            rewriteToServo = true;
            reverse = rev;
        }
    }
    bool getReverse()
    {
        return reverse;
    }
    void setAngleLimits(float _minAngleLimit, float _maxAngleLimit)
    {
        setMinAngleLimit(_minAngleLimit);
        setMaxAngleLimit(_maxAngleLimit);
    }
    void setMinAngleLimit(float _minAngleLimit)
    {
        if (minAngleLimit != _minAngleLimit) {
            rewriteToServo = true;
            minAngleLimit = _minAngleLimit;
        }
        dL.setPosLimits(min(minAngleLimit, maxAngleLimit), max(minAngleLimit, maxAngleLimit));
    }
    void setMaxAngleLimit(float _maxAngleLimit)
    {
        if (maxAngleLimit != _maxAngleLimit) {
            rewriteToServo = true;
            maxAngleLimit = _maxAngleLimit;
        }
        dL.setPosLimits(min(minAngleLimit, maxAngleLimit), max(minAngleLimit, maxAngleLimit));
    }
    float getMinAngleLimit()
    {
        return minAngleLimit;
    }
    float getMaxAngleLimit()
    {
        return maxAngleLimit;
    }
    float getAngleLimitRange()
    {
        return maxAngleLimit - minAngleLimit;
    }

    void setSetAngles(float _minSetAngle, float _maxSetAngle)
    {
        setMinSetAngle(_minSetAngle);
        setMaxSetAngle(_maxSetAngle);
    }
    void setMinSetAngle(float _minSetAngle)
    {
        if (minSetAngle != _minSetAngle) {
            rewriteToServo = true;
            minSetAngle = _minSetAngle;
        }
    }
    void setMaxSetAngle(float _maxSetAngle)
    {
        if (maxSetAngle != _maxSetAngle) {
            rewriteToServo = true;
            maxSetAngle = _maxSetAngle;
        }
    }

    float getAccelLimit()
    {
        return dL.getAccelLimit();
    }
    float getVelLimit()
    {
        return dL.getVelLimit();
    }
    void setAccelLimit(float accelLim)
    {
        dL.setAccelLimit(accelLim);
    }
    void setVelLimit(float velLim)
    {
        dL.setVelLimit(velLim);
    }
    void setVelAccelLimits(float velLim, float accelLim)
    {
        dL.setVelAccelLimits(velLim, accelLim);
    }
    /**
     * @brief  sets servo position, leaves target where it was
     * @param  pos: (float)
     * @param  run: (bool) default: true, true = call run() in this function, false = you'll call run() yourself
     * 
     */
    void setPosition(float pos, bool _run)
    {
        if (pos != dL.getPosition()) {
            wake();
            dL.setPosition(pos);
        }
        if (_run)
            run();
    }

    /**
     * @brief  microseconds for shortest servo pulse
     * @param  value: (int)
     */
    void setMinServoValue(int value)
    {
        if (servo.getMinServoValue() != value) {
            rewriteToServo = true;
            servo.setMinServoValue(value);
        }
    }
    /**
     * @brief  microseconds for longest servo pulse
     * @param  value: (int)
     */
    void setMaxServoValue(int value)
    {
        if (servo.getMaxServoValue() != value) {
            rewriteToServo = true;
            servo.setMaxServoValue(value);
        }
    }

    /**
     * @brief  set the settings for short and long servo pulses at the same time
     * @param  min: (int) microseconds, often 544
     * @param  max: (int) microseconds, often 2400
     */
    void setServoRangeValues(int min, int max)
    {
        setMinServoValue(min);
        setMaxServoValue(max);
    }

    /**
     * @brief  returns setting for microseconds for shortest servo pulse
     * @retval  (int)
     */
    int getMinServoRangeValue()
    {
        return servo.getMinServoValue();
    }
    /**
     * @brief  returns setting for microseconds for longest servo pulse
     * @retval  (int)
     */
    int getMaxServoRangeValue()
    {
        return servo.getMaxServoValue();
    }
    /**
     * @brief  returns difference (in microseconds) between longest and shortest servo pulse settings
     * @retval  (int)
     */
    int getServoValueRange()
    {
        return servo.getServoValueRange();
    }

protected:
    void writeAngleToServo(float ang)
    {
        ang = constrain(ang, min(minAngleLimit, maxAngleLimit), max(minAngleLimit, maxAngleLimit));
        if ((reverse) != (minAngleLimit < maxAngleLimit)) {
            ang = floatMap(ang, maxSetAngle, minSetAngle, servo.getMinRange(), servo.getMaxRange());
        } else { //not reversed
            ang = floatMap(ang, minSetAngle, maxSetAngle, servo.getMinRange(), servo.getMaxRange());
        }
        if (rewriteToServo && enabled && !sleeping) {
            servo.set(ang);
            rewriteToServo = false;
        }
    }
    float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};

#include "JServoControllerAdvanced.h"
#include "JServoControllerGentle.h"
#include "JServoControllerStallProtected.h"
#include "JServoCurrentSensor.h"

#endif
