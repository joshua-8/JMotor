#ifndef J_DRIVETRAIN_CONTROLLER_BASIC_H
#define J_DRIVETRAIN_CONTROLLER_BASIC_H
#include "JDrivetrain/JDrivetrain.h"
#include <Arduino.h>
class JDrivetrainControllerBasic : public JDrivetrain {
protected:
    JDrivetrain& drivetrain;
    twoDTransform distError;
    bool controlled;
    bool posMode;
    unsigned long lastCalcMillis;

    twoDTransform velTarget;

public:
    Derivs_Limiter YLimiter;
    Derivs_Limiter RZLimiter;
    Derivs_Limiter XLimiter;

    JDrivetrainControllerBasic(JDrivetrain& _drivetrain, twoDTransform _velLimit, twoDTransform _accelLimit, twoDTransform _distError)
        : drivetrain(_drivetrain)
        , distError(_distError)
        , YLimiter(Derivs_Limiter(_velLimit.y, _accelLimit.y))
        , RZLimiter(Derivs_Limiter(_velLimit.rz, _accelLimit.rz))
        , XLimiter(Derivs_Limiter(_velLimit.x, _accelLimit.x))
    {
        YLimiter.setPreventGoingWrongWay(false);
        XLimiter.setPreventGoingWrongWay(false);
        RZLimiter.setPreventGoingWrongWay(false);
        controlled = false;
        posMode = false;
        lastCalcMillis = 0;
    }

    void run()
    {
        float time = (micros() - lastCalcMillis) / 1000000.0;
        if (lastCalcMillis == 0) {
            time = 0;
            lastCalcMillis = micros();
        }
        if (time == 0) {
            return;
        }
        lastCalcMillis = micros();

        if (controlled) {
            if (posMode) {
                YLimiter.calc();
                RZLimiter.calc();
                XLimiter.calc();

                twoDTransform dist = getDist();
                YLimiter.setPosition(constrain(YLimiter.getPosition(), dist.y - distError.y, dist.y + distError.y));
                RZLimiter.setPosition(constrain(RZLimiter.getPosition(), dist.rz - distError.rz, dist.rz + distError.rz));
                XLimiter.setPosition(constrain(XLimiter.getPosition(), dist.x - distError.x, dist.x + distError.x));

                drivetrain.setDistSetpoint({ YLimiter.getPosition(), RZLimiter.getPosition(), XLimiter.getPosition() }, false);
            } else {
                if (YLimiter.getVelocity() != velTarget.y) {
                    YLimiter.setVelocity(YLimiter.getVelocity() + constrain(velTarget.y - YLimiter.getVelocity(), -YLimiter.getAcceleration() * time, YLimiter.getAcceleration() * time));
                }
                if (RZLimiter.getVelocity() != velTarget.rz) {
                    RZLimiter.setVelocity(RZLimiter.getVelocity() + constrain(velTarget.rz - RZLimiter.getVelocity(), -RZLimiter.getAcceleration() * time, RZLimiter.getAcceleration() * time));
                }
                if (XLimiter.getVelocity() != velTarget.x) {
                    XLimiter.setVelocity(XLimiter.getVelocity() + constrain(velTarget.x - XLimiter.getVelocity(), -XLimiter.getAcceleration() * time, XLimiter.getAcceleration() * time));
                }

                YLimiter.setVelocity(constrain(YLimiter.getVelocity(), -YLimiter.getVelLimit(), YLimiter.getVelLimit()));
                RZLimiter.setVelocity(constrain(RZLimiter.getVelocity(), -RZLimiter.getVelLimit(), RZLimiter.getVelLimit()));
                XLimiter.setVelocity(constrain(XLimiter.getVelocity(), -XLimiter.getVelLimit(), XLimiter.getVelLimit()));

                drivetrain.setVel({ YLimiter.getVelocity(), RZLimiter.getVelocity(), XLimiter.getVelocity() });
            }
        }

        drivetrain.run();
    }

    void moveVel(twoDTransform _vel, bool _run = false)
    {
        if (posMode || !controlled) {
            twoDTransform vel = getVel();
            YLimiter.setVelocity(vel.y);
            RZLimiter.setVelocity(vel.rz);
            XLimiter.setVelocity(vel.x);
        }

        controlled = true;
        posMode = false;

        if (_run)
            run();
    }

    void movePos(twoDTransform _pos, bool _run = false)
    {
        if (!posMode || !controlled) {
            YLimiter.resetTime();
            XLimiter.resetTime();
            RZLimiter.resetTime();
            twoDTransform vel = getVel();
            YLimiter.setVelocity(vel.y);
            RZLimiter.setVelocity(vel.rz);
            XLimiter.setVelocity(vel.x);
            twoDTransform dist = getDist();
            YLimiter.setPosition(dist.y);
            RZLimiter.setPosition(dist.rz);
            XLimiter.setPosition(dist.x);
        }
        YLimiter.setTarget(_pos.y);
        RZLimiter.setTarget(_pos.rz);
        XLimiter.setTarget(_pos.x);

        controlled = true;
        posMode = true;

        if (_run)
            run();
    }

    bool getPosMode()
    {
        return posMode;
    }

    bool getIsControlled()
    {
        return controlled;
    }

    ////JDrivetrain methods:
    void setVel(twoDTransform _vel, bool _run = false)
    {
        controlled = false;
        drivetrain.setVel(_vel, false);
        if (_run)
            run();
    }
    void setDistSetpoint(twoDTransform _dist, bool _run = false)
    {
        controlled = false;
        drivetrain.setDistSetpoint(_dist, false);
        if (_run)
            run();
    }
    void setDistDelta(twoDTransform _dist, bool _run = false)
    {
        controlled = false;
        drivetrain.setDistDelta(_dist, false);
        if (_run)
            run();
    }
    void resetDist()
    {
        twoDTransform dist = getDist();
        YLimiter.setTarget(YLimiter.getTarget() - dist.y);
        RZLimiter.setTarget(RZLimiter.getTarget() - dist.rz);
        XLimiter.setTarget(XLimiter.getTarget() - dist.x);
        drivetrain.resetDist();
    }
    twoDTransform getVel(bool _run = false)
    {
        if (_run)
            run();
        return drivetrain.getVel(false);
    }
    twoDTransform getDist(bool _run = false)
    {
        if (_run)
            run();
        return drivetrain.getDist(false);
    }
    twoDTransform getMaxVel()
    {
        return drivetrain.getMaxVel();
    }
    void setMotorVels(float vels[], bool _run = false)
    {
        controlled = false;
        drivetrain.setMotorVels(vels, false);
        if (_run)
            run();
    }
    void setMotorDistSetpoints(float vels[], bool _run = false)
    {
        controlled = false;
        drivetrain.setMotorDistSetpoints(vels, false);
        if (_run)
            run();
    }
    void setMotorDistDeltas(float vels[], bool _run = false)
    {
        controlled = false;
        drivetrain.setMotorDistDeltas(vels, false);
        if (_run)
            run();
    }

    float getMotorVel(unsigned char i)
    {
        return drivetrain.getMotorVel(i);
    }
    unsigned char getNumberMotors()
    {
        return drivetrain.getNumberMotors();
    }

    bool setEnable(bool _enable)
    {
        return drivetrain.setEnable(_enable);
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
        return drivetrain.getEnable();
    }
};
#endif