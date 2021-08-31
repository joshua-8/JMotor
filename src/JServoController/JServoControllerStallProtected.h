#ifndef J_SERVO_CONTROLLER_STALL_PROTECTED_H
#define J_SERVO_CONTROLLER_STALL_PROTECTED_H
#include "JServoControllerAdvanced.h"
#include "JServoStallSensing.h"
#include <Arduino.h>
/**
 * @brief  this child class of JServoController uses JServoStallSensing to reduce servo power
 */
class JServoControllerStallProtected : public JServoControllerAdvanced {
protected:
    bool stalled;
    float unStallThreshold;
    float stallThreshold;
    bool stallProtected;
    unsigned long stallStateChangeMillis;
    bool stallProtectionActivated;
    unsigned long stallActivateTimeout;
    unsigned long stallDeactivateTimeout;

public:
    JServoStallSensing& stallSensor;
    JServoControllerStallProtected(JServoControllerAdvanced advancedServo, JServoStallSensing& servoStallSensor, float _unStallThreshold, float _stallThreshold, unsigned long _stallActivateTimeout, unsigned long _stallDeactivateTimeout)
        : JServoControllerAdvanced(advancedServo)
        , stallSensor(servoStallSensor)
    {
        unStallThreshold = constrain(_unStallThreshold, 0, 1);
        stallThreshold = constrain(_stallThreshold, 0, 1);
        stalled = false;
        stallProtected = true;
        stallActivateTimeout = _stallActivateTimeout;
        stallDeactivateTimeout = _stallDeactivateTimeout;
    }
    virtual void run()
    {
        float measurementTemp = stallSensor.getMeasurement(true);
        if (measurementTemp > stallThreshold) {
            if (!stalled) {
                stalled = true;
                stallStateChangeMillis = millis();
            }
        }
        if (measurementTemp < unStallThreshold) {
            if (stalled) {
                stalled = false;
                stallStateChangeMillis = millis();
            }
        }
        if ((stalled && (millis() - stallStateChangeMillis >= stallActivateTimeout)) || (!stalled && (millis() - stallStateChangeMillis >= stallDeactivateTimeout))) {
            stallProtectionActivated = stalled;
        }

        if (stallProtectionActivated) {
            if (stallProtected && !weakened) {
                setStrengthWeak();
            }
        }
        if (!stallProtectionActivated) {
            if (stallProtected && weakened) {
                setStrengthNormal();
            }
        }

        JServoControllerAdvanced::run();
    }
    bool isStallProtectionActivated()
    {
        return stallProtectionActivated;
    }
    unsigned long getTimeSinceStallStateChangeMillis()
    {
        return millis() - stallStateChangeMillis;
    }
    bool isStalled()
    {
        return stalled;
    }
    bool getStallProtectionEnable()
    {
        return stallProtected;
    }
    void setStallProtectionEnable(bool enableStallProtection)
    {
        stallProtected = enableStallProtection;
        if (stallProtected == false) {
            if (stalled) {
                setStrengthNormal();
            }
        }
    }
};
#endif