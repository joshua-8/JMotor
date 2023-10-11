#ifndef JDRIVETRAIN_MECANUM_H
#define JDRIVETRAIN_MECANUM_H
#include "JDrivetrain.h"
#include "JMotorController/JMotorController.h"
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 * @brief Drivetrain that controls a mecanum wheeled robot
 */
// TODO: COMMENT
class JDrivetrainMecanum : public JDrivetrain {
public:
    struct Motors {
        JMotorController& FRmotor;
        JMotorController& FLmotor;
        JMotorController& BLmotor;
        JMotorController& BRmotor;
    } motors;

    JDrivetrainMecanum(JMotorController& _FRmotor, JMotorController& _FLmotor, JMotorController& _BLmotor, JMotorController& _BRmotor, JTwoDTransform _robotToWheelScalar)
        : motors({ _FRmotor, _FLmotor, _BLmotor, _BRmotor })
    {
        robotToWheelScalar = _robotToWheelScalar;
    }
    void run()
    {
        motors.FRmotor.run();
        motors.FLmotor.run();
        motors.BLmotor.run();
        motors.BRmotor.run();
    }
    bool setEnable(bool _enable)
    {
        bool ret1 = motors.FRmotor.setEnable(_enable);
        bool ret2 = motors.FLmotor.setEnable(_enable);
        bool ret3 = motors.BLmotor.setEnable(_enable);
        bool ret4 = motors.BRmotor.setEnable(_enable);
        return ret1 || ret2 || ret3 || ret4;
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
        bool ret1 = motors.FRmotor.getEnable();
        bool ret2 = motors.FLmotor.getEnable();
        bool ret3 = motors.BLmotor.getEnable();
        bool ret4 = motors.BRmotor.getEnable();
        return ret1 || ret2 || ret3 || ret4;
    }
    JTwoDTransform getVel(bool _run = false)
    {
        if (_run)
            run();
        MotorFloats m;
        m.FRmotor = motors.FRmotor.getVel();
        m.FLmotor = motors.FLmotor.getVel();
        m.BLmotor = motors.BLmotor.getVel();
        m.BRmotor = motors.BRmotor.getVel();
        JTwoDTransform t;
        toTransform(m, t);
        return t;
    }
    JTwoDTransform getDist(bool _run = false)
    {
        if (_run)
            run();
        MotorFloats m;
        m.FRmotor = motors.FRmotor.getPos();
        m.FLmotor = motors.FLmotor.getPos();
        m.BLmotor = motors.BLmotor.getPos();
        m.BRmotor = motors.BRmotor.getPos();
        JTwoDTransform t;
        toTransform(m, t);
        return t;
    }
    void setVel(JTwoDTransform _vel, bool _run = false)
    {
        MotorFloats m;
        toWheels(_vel, m);
        motors.FRmotor.setVel(m.FRmotor, false);
        motors.FLmotor.setVel(m.FLmotor, false);
        motors.BLmotor.setVel(m.BLmotor, false);
        motors.BRmotor.setVel(m.BRmotor, false);
        if (_run)
            run();
    }
    void setDistSetpoint(JTwoDTransform _dist, bool _run = false)
    {
        MotorFloats m;
        toWheels(_dist, m);
        motors.FRmotor.setPosSetpoint(m.FRmotor, false);
        motors.FLmotor.setPosSetpoint(m.FLmotor, false);
        motors.BLmotor.setPosSetpoint(m.BLmotor, false);
        motors.BRmotor.setPosSetpoint(m.BRmotor, false);
        if (_run)
            run();
    }
    void setDistDelta(JTwoDTransform _dist, bool _run = false)
    {
        MotorFloats m;
        toWheels(_dist, m);
        motors.FRmotor.setPosDelta(m.FRmotor, false);
        motors.FLmotor.setPosDelta(m.FLmotor, false);
        motors.BLmotor.setPosDelta(m.BLmotor, false);
        motors.BRmotor.setPosDelta(m.BRmotor, false);
        if (_run)
            run();
    }
    void resetDist()
    {
        motors.FRmotor.resetPos();
        motors.FLmotor.resetPos();
        motors.BLmotor.resetPos();
        motors.BRmotor.resetPos();
    }
    JTwoDTransform getMaxVel()
    {
        // assumes all the max vels are similar, doesn't find a minimum
        // can't use toTransform since toTransform adds negatives that causes calculations to say x and theta max vel are zero
        JTwoDTransform t;
        float v = abs(motors.FRmotor.getMaxVel()) + abs(motors.FLmotor.getMaxVel()) + abs(motors.BLmotor.getMaxVel()) + abs(motors.BRmotor.getMaxVel());
        t.x = v / 4.0 / robotToWheelScalar.x;
        t.y = v / 4.0 / robotToWheelScalar.y;
        t.theta = v / 4.0 / robotToWheelScalar.theta;
        return t;
    }
    float getMotorVel(unsigned char i)
    {
        if (i == 0)
            return motors.FRmotor.getVel();
        if (i == 1)
            return motors.FLmotor.getVel();
        if (i == 2)
            return motors.BLmotor.getVel();
        if (i == 3)
            return motors.BRmotor.getVel();
        return 0;
    }
    void setMotorVel(float vel, unsigned char i, bool _run = false)
    {
        if (i == 0)
            motors.FRmotor.setVel(vel, _run);
        if (i == 1)
            motors.FLmotor.setVel(vel, _run);
        if (i == 2)
            motors.BLmotor.setVel(vel, _run);
        if (i == 3)
            motors.BRmotor.setVel(vel, _run);
    }
    void setMotorDistSetpoint(float distSetpoint, unsigned char i, bool _run = false)
    {
        if (i == 0)
            motors.FRmotor.setPosSetpoint(distSetpoint, _run);
        if (i == 1)
            motors.FLmotor.setPosSetpoint(distSetpoint, _run);
        if (i == 2)
            motors.BLmotor.setPosSetpoint(distSetpoint, _run);
        if (i == 3)
            motors.BRmotor.setPosSetpoint(distSetpoint, _run);
    }
    void setMotorDistDelta(float distDelta, unsigned char i, bool _run = false)
    {
        if (i == 0)
            motors.FRmotor.setPosDelta(distDelta, _run);
        if (i == 1)
            motors.FLmotor.setPosDelta(distDelta, _run);
        if (i == 2)
            motors.BLmotor.setPosDelta(distDelta, _run);
        if (i == 3)
            motors.BRmotor.setPosDelta(distDelta, _run);
    }
    unsigned char getNumberMotors()
    {
        return 4;
    }
    JTwoDTransform getRobotToWheelScalar()
    {
        return robotToWheelScalar;
    }
    void setRobotToWheelScalar(JTwoDTransform scalar)
    {
        robotToWheelScalar = scalar;
    }

protected:
    JTwoDTransform robotToWheelScalar;
    struct MotorFloats {
        float FRmotor;
        float FLmotor;
        float BLmotor;
        float BRmotor;
    };
    void toWheels(JTwoDTransform t, MotorFloats& m)
    {
        m.FRmotor = t.x * robotToWheelScalar.x + t.y * robotToWheelScalar.y + t.theta * robotToWheelScalar.theta;
        m.FLmotor = t.x * robotToWheelScalar.x - t.y * robotToWheelScalar.y - t.theta * robotToWheelScalar.theta;
        m.BLmotor = t.x * robotToWheelScalar.x + t.y * robotToWheelScalar.y - t.theta * robotToWheelScalar.theta;
        m.BRmotor = t.x * robotToWheelScalar.x - t.y * robotToWheelScalar.y + t.theta * robotToWheelScalar.theta;
    }
    void toTransform(MotorFloats m, JTwoDTransform& w)
    {
        w.x = (m.FRmotor + m.FLmotor + m.BLmotor + m.BRmotor) / 4.0 / robotToWheelScalar.x;
        w.y = (m.FRmotor - m.FLmotor + m.BLmotor - m.BRmotor) / 4.0 / robotToWheelScalar.y;
        w.theta = (m.FRmotor - m.FLmotor - m.BLmotor + m.BRmotor) / 4.0 / robotToWheelScalar.theta;
    }
};
#endif // JDRIVETRAIN_MECANUM_H
