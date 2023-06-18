#ifndef JSWERVE_MODULE_DIFFERENTIAL_H
#define JSWERVE_MODULE_DIFFERENTIAL_H
#include "JEncoder/JEncoder.h"
#include "JMotorController/JMotorControllerBasic.h"
#include "JSwerveAngleControlLoop.h"
#include "JSwerveModule.h"
#include <Arduino.h>
class JSwerveModuleDifferential : public JSwerveModule {
public:
    JMotorControllerBasic& m1;
    JMotorControllerBasic& m2;
    JEncoder& angleEncoder;
    JSwerveAngleControlLoop& angleControlLoop;

    float driveRatio = 1;
    float steeringRatio = 1;
    float angleOffset = 0;

    float speedSetpoint = 0;
    float angleSetpoint = 0;

    /**
     * @brief
     * @param  _m1: positive speed drives forward and steers CCW if viewed from above, just swap which motor is m1 and m2 or reverse a motor if this is not the case
     * @param  _m2: positive speed drives forward and steers CW if viewed from above, just swap which motor is m1 and m2 or reverse a motor if this is not the case
     * @param  _angleEncoder: JEncoder, should be absolute encoder, from 0-360 degrees for a revolution
     * @param  _angleControlLoop: JSwerveAngleControlLoop object
     * @param  _driveRatio: motor revolutions per drive distance unit
     * @param  _steeringRatio: motor revolutions per steering revolution
     * @param  _angleOffset: degrees, default=0 added to encoder reading, adjust until angle is zero when wheel is straight forwards
     */
    JSwerveModuleDifferential(JMotorControllerBasic& _m1, JMotorControllerBasic& _m2, JEncoder& _angleEncoder, JSwerveAngleControlLoop& _angleControlLoop,
        float _driveRatio, float _steeringRatio, float _angleOffset = 0)
        : m1(_m1)
        , m2(_m2)
        , angleEncoder(_angleEncoder)
        , angleControlLoop(_angleControlLoop)
    {
        driveRatio = _driveRatio;
        steeringRatio = _steeringRatio;
        angleOffset = _angleOffset;
    }

    void set(float _speed, float _angle, bool _run = true)
    {
        speedSetpoint = _speed;
        angleSetpoint = _angle;

        if (_run) {
            run();
        }
    }

    float getAngle()
    {
        return angleEncoder.getPos() + angleOffset;
    }

    long zeroTurnCounter()
    {
        return angleEncoder.zeroCounter();
    }

    bool setEnable(bool _enable)
    {
        bool m1state = m1.setEnable(_enable);
        bool m2state = m2.setEnable(_enable);
        if (m1state || m2state) {
            angleSetpoint = getAngle();
            speedSetpoint = 0;
        }
        return m1state || m2state;
    }

    bool getEnable()
    {
        bool m1state = m1.getEnable();
        bool m2state = m2.getEnable();
        return m1state || m2state;
    }

    float getMaxSpeed()
    {
        return min(m1.getMaxVel(), m2.getMaxVel()) / driveRatio;
    }

    void run()
    {
        angleEncoder.run();

        float maxSpeed = getMaxSpeed();
        speedSetpoint = constrain(speedSetpoint, -maxSpeed, maxSpeed);

        float m1Speed = speedSetpoint * driveRatio;
        float m2Speed = speedSetpoint * driveRatio;

        float angleCtrlLoopOut = angleControlLoop.calculate(angleSetpoint - getAngle());

        m1Speed += angleCtrlLoopOut * steeringRatio;
        m2Speed -= angleCtrlLoopOut * steeringRatio;

        float overspeed = max(abs(m1Speed) / m1.getMaxVel(), abs(m2Speed) / m2.getMaxVel());

        m1.setVelTarget(m1Speed / overspeed);
        m2.setVelTarget(m2Speed / overspeed);

        m1.run();
        m2.run();
    }
};

#endif // JSWERVE_MODULE_DIFFERENTIAL_H
