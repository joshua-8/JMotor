#ifndef J_SERVO_CONTROLLER_GENTLE_H
#define J_SERVO_CONTROLLER_GENTLE_H
#include "JServoController.h"
#include "JServoStallSensing.h"
/**
 * @brief  JServoControllerGentle is an extension of JServoController that uses JServoStallSensing to make a servo grip with a specific amount of force
 */
class JServoControllerGentle : public JServoController {
protected:
    bool gripDirPos;
    float targetForce;
    float gripSpeed;

public:
    /**
     * @brief  true=gripping, false = other servo movmement set
     */
    bool grippingGently;
    JServoStallSensing& stallSensor;

    /**
     * @brief  Constructor for JServoController Gentle (a child class of JServoController)
     * @param  servo: 
     * @param  servoStallSensor: 
     * @param  _targetForce: target force as measured by JServoStallSensing (if set too low, gripping may not work since just moving the servo could take more than the set power)
     * @param  _gripSpeed: 
     */
    JServoControllerGentle(JServoControllerAdvanced servo, JServoStallSensing& servoStallSensor, float _targetForce = .05, float _gripSpeed = 30)
        : JServoController(servo)
        , stallSensor(servoStallSensor)
    {
        gripSpeed = max(_gripSpeed, (float)0);
        grippingGently = false;
        gripDirPos = true;
        targetForce = max(_targetForce, (float)0);
    }

    virtual void run()
    {
        stallSensor.run();
        if (grippingGently) {
            if (stallSensor.getMeasurement(false) < targetForce) {
                JServoController::setAngleImmediateInc((gripDirPos ? 1.0 : -1.0) * gripSpeed * dL.getTimeInterval(), false);
            } else if (stallSensor.getMeasurement(false) > targetForce) {
                JServoController::setAngleImmediateInc((!gripDirPos ? 1.0 : -1.0) * gripSpeed * dL.getTimeInterval(), false);
            } else {
                JServoController::setAngleImmediateInc(0, false);
            }
        }
        JServoController::run();
    }
    /**
     * @brief  have the servo move until it is pushing with a set amount of force
     * @param  _dirPos: (bool) true = move in positive direction, false = move in negative direction
     * @param  _targetForce: (float, default NAN) if not NAN, override default setting for how hard to grip
     * @param  _gripSpeed: (float, default NAN) if not NAN, override default setting for how fast to move
     * @param  _run: (bool, default true) true = call run() in this function, false = you'll call run() yourself
     */
    void gripGently(bool _dirPos, float _targetForce = NAN, float _gripSpeed = NAN, bool _run = true)
    {
        if (isnan(_targetForce))
            _targetForce = targetForce;
        if (isnan(_targetForce))
            _targetForce = targetForce;
        gripDirPos = _dirPos;
        grippingGently = true;
        if (_run)
            run();
    }

    void setPosition(float pos, bool _run)
    {
        grippingGently = false;
        JServoController::setPosition(pos, _run);
    }
    void setAngleImmediate(float angle, bool _run = true)
    {
        grippingGently = false;
        JServoController::setAngleImmediate(angle, _run = true);
    }
    void setAngleImmediateInc(float angleDiff, bool _run = true)
    {
        grippingGently = false;
        JServoController::setAngleImmediateInc(angleDiff, _run);
    }
    void setAngleSmoothed(float angle, bool _run = true)
    {
        grippingGently = false;
        JServoController::setAngleSmoothed(angle, _run);
    }
};
#endif
