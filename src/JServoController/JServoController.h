#ifndef J_SERVO_CONTROLLER_H
#define J_SERVO_CONTROLLER_H
#include "Derivs_Limiter.h"
#include "JMotorDriver/JMotorDriverServo.h"
#include <Arduino.h>
/**
 * @brief  
 * @note   
 * @retval None
 */
class JServoController {
protected:
    /**
 * @brief  
 * @note   
 * @retval None
 */
    JMotorDriverServo& servo;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    int disableTimeout;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    float minAngleLimit;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    float maxAngleLimit;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    float minSetAngle;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    float maxSetAngle;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    bool reverse;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    unsigned long lastMovedMillis;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    bool enabled;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    bool sleeping;
    /**
     * @brief  
     * @note   
     * @retval None
     */
    bool rewriteToServo;

public:
    /**
     * @brief  
     * @note   
     * @retval None
     */
    Derivs_Limiter dL;
    /**
     * @brief  
     * @note   
     * @param  _servo: 
     * @param  _reverse: 
     * @param  velLimit: 
     * @param  accelLimit: 
     * @param  _disableTimeout: 
     * @param  _minAngleLimit: 
     * @param  _maxAngleLimit: 
     * @param  _pos: 
     * @param  minServoVal: 
     * @param  maxServoVal: 
     * @param  _minSetAngle: 
     * @param  _maxSetAngle: 
     * @retval 
     */
    JServoController(JMotorDriverServo& _servo, bool _reverse = false, float velLimit = INFINITY, float accelLimit = INFINITY, unsigned long _disableTimeout = 0, float _minAngleLimit = 0, float _maxAngleLimit = 180, float _pos = 90, int minServoVal = 544, int maxServoVal = 2400, float _minSetAngle = 0, float _maxSetAngle = 180)
        : servo(_servo)
        , dL(Derivs_Limiter(max(velLimit, float(0.0)), max(accelLimit, float(0.0)), _pos, _pos))
    {
        enabled = false;
        sleeping = false;
        lastMovedMillis = 0;
        minAngleLimit = _minAngleLimit;
        maxAngleLimit = _maxAngleLimit;
        servo.setServoValues(minServoVal, maxServoVal);
        disableTimeout = _disableTimeout;
        reverse = _reverse;
        dL.setPosition(_pos);
        minSetAngle = _minSetAngle;
        maxSetAngle = maxSetAngle;
        servo.setServoValues(minServoVal, maxServoVal);
        rewriteToServo = true;
    }
    void run()
    {
        dL.calc();
        if (dL.getVelocity() != 0) {
            lastMovedMillis = millis();
        }
        if (disableTimeout != 0) {
            sleeping = millis() - lastMovedMillis > disableTimeout;
            if (sleeping) {
                if (servo.getEnable() == true) {
                    servo.disable();
                }
            } else { //activate
                if (servo.getEnable() == false) {
                    servo.enable();
                    rewriteToServo = true;
                }
            }
        }
        if (dL.getPosDelta() != 0) {
            rewriteToServo = true;
        }
        if (servo.getEnable()) {
            writeAngleToServo(dL.getPosition());
        }
    }
    void setAngle(float angle, bool _run = true)
    {
        dL.setTarget(angle);
        dL.setPosition(angle);
        if (_run)
            run();
    }
    void setAngleSmoothed(float angle, bool _run = true)
    {
        dL.setTarget(angle);
        if (_run)
            run();
    }
    void setDisableTimeout(unsigned long _timeout)
    {
        disableTimeout = _timeout;
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
    bool setEnable(bool enable)
    {
        enabled = enable;
        return servo.setEnable(enable);
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
    float getTargetPos()
    {
        return dL.getTarget();
    }
    float getPos()
    {
        return dL.getPosition();
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
        rewriteToServo = true;
        reverse = rev;
    }
    void setAngleLimits(float _minAngleLimit, float _maxAngleLimit)
    {
        setMinAngleLimit(_minAngleLimit);
        setMaxAngleLimit(_maxAngleLimit);
    }
    void setMinAngleLimit(float _minAngleLimit)
    {
        rewriteToServo = true;
        minAngleLimit = _minAngleLimit;
    }
    void setMaxAngleLimit(float _maxAngleLimit)
    {
        rewriteToServo = true;
        maxAngleLimit = _maxAngleLimit;
    }

    void setSetAngles(float _minSetAngle, float _maxSetAngle)
    {
        setMinSetAngle(_minSetAngle);
        setMaxSetAngle(_maxSetAngle);
    }
    void setMinSetAngle(float _minSetAngle)
    {
        rewriteToServo = true;
        minSetAngle = _minSetAngle;
    }
    void setMaxSetAngle(float _maxSetAngle)
    {
        rewriteToServo = true;
        maxSetAngle = _maxSetAngle;
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
    void setVelocity(float _vel)
    {
        rewriteToServo = true;
        dL.setVelocity(_vel);
    }

    /**
     * @brief  microseconds for shortest servo pulse
     * @param  value: (int)
     */
    void setMinServoValue(int value)
    {
        rewriteToServo = true;
        servo.setMinServoValue(value);
    }
    /**
     * @brief  microseconds for longest servo pulse
     * @param  value: (int)
     */
    void setMaxServoValue(int value)
    {
        rewriteToServo = true;
        servo.setMaxServoValue(value);
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
        if ((reverse) == (minAngleLimit < maxAngleLimit)) {
            ang = map(ang * 100, maxSetAngle * 100, minSetAngle * 100, servo.getMinRange(), servo.getMaxRange());
        } else { //not reversed
            ang = map(ang * 100, minSetAngle * 100, maxSetAngle * 100, servo.getMinRange(), servo.getMaxRange());
        }
        if (rewriteToServo) {
            servo.set(ang);
            rewriteToServo = false;
        }
    }
};
#endif
