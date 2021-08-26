#ifndef J_DRIVETRAIN_CONTROLLER_BASIC_H
#define J_DRIVETRAIN_CONTROLLER_BASIC_H
#include "JDrivetrain/JDrivetrain.h"
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 * @brief  basic drivetrain controller, supports smoothed movements on each axis
 * @note each axis is separate, this drivetrain controller doesn't use distances moved on each axis to actually know its position
 */
class JDrivetrainControllerBasic : public JDrivetrain {
protected:
    bool controlled;
    bool distMode;
    unsigned long lastCalcMillis;

    JTwoDTransform velTarget;

public:
    JDrivetrain& drivetrain;
    Derivs_Limiter YLimiter;
    Derivs_Limiter RZLimiter;
    Derivs_Limiter XLimiter;
    JTwoDTransform distError;

    JDrivetrainControllerBasic(JDrivetrain& _drivetrain, JTwoDTransform _velLimit, JTwoDTransform _accelLimit, JTwoDTransform _distError)
        : drivetrain(_drivetrain)
        , YLimiter(Derivs_Limiter(_velLimit.y, _accelLimit.y))
        , RZLimiter(Derivs_Limiter(_velLimit.rz, _accelLimit.rz))
        , XLimiter(Derivs_Limiter(_velLimit.x, _accelLimit.x))
        , distError(_distError)
    {
        YLimiter.setPreventGoingWrongWay(false);
        XLimiter.setPreventGoingWrongWay(false);
        RZLimiter.setPreventGoingWrongWay(false);
        YLimiter.setPreventGoingTooFast(true);
        RZLimiter.setPreventGoingTooFast(true);
        XLimiter.setPreventGoingTooFast(true);
        controlled = false;
        distMode = false;
        lastCalcMillis = 0;
    }

    /**
     * @brief  run as frequently as possible
     */
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
        if (getEnable()) {
            if (controlled) {
                if (distMode) {
                    YLimiter.calc();
                    RZLimiter.calc();
                    XLimiter.calc();

                    JTwoDTransform dist = getDist();
                    YLimiter.setPosition(constrain(YLimiter.getPosition(), dist.y - distError.y, dist.y + distError.y));
                    RZLimiter.setPosition(constrain(RZLimiter.getPosition(), dist.rz - distError.rz, dist.rz + distError.rz));
                    XLimiter.setPosition(constrain(XLimiter.getPosition(), dist.x - distError.x, dist.x + distError.x));

                    drivetrain.setDistSetpoint({ YLimiter.getPosition(), RZLimiter.getPosition(), XLimiter.getPosition() }, false);
                } else {
                    if (YLimiter.getVelocity() != velTarget.y) {
                        YLimiter.setVelocity(YLimiter.getVelocity() + constrain(velTarget.y - YLimiter.getVelocity(), -YLimiter.getAccelLimit() * time, YLimiter.getAccelLimit() * time));
                    }
                    if (RZLimiter.getVelocity() != velTarget.rz) {
                        RZLimiter.setVelocity(RZLimiter.getVelocity() + constrain(velTarget.rz - RZLimiter.getVelocity(), -RZLimiter.getAccelLimit() * time, RZLimiter.getAccelLimit() * time));
                    }
                    if (XLimiter.getVelocity() != velTarget.x) {
                        XLimiter.setVelocity(XLimiter.getVelocity() + constrain(velTarget.x - XLimiter.getVelocity(), -XLimiter.getAccelLimit() * time, XLimiter.getAccelLimit() * time));
                    }

                    YLimiter.setVelocity(constrain(YLimiter.getVelocity(), -YLimiter.getVelLimit(), YLimiter.getVelLimit()));
                    RZLimiter.setVelocity(constrain(RZLimiter.getVelocity(), -RZLimiter.getVelLimit(), RZLimiter.getVelLimit()));
                    XLimiter.setVelocity(constrain(XLimiter.getVelocity(), -XLimiter.getVelLimit(), XLimiter.getVelLimit()));

                    YLimiter.setVelocity(constrain(YLimiter.getVelocity(), -getMaxVel().y, getMaxVel().y));
                    RZLimiter.setVelocity(constrain(RZLimiter.getVelocity(), -getMaxVel().rz, getMaxVel().rz));
                    XLimiter.setVelocity(constrain(XLimiter.getVelocity(), -getMaxVel().x, getMaxVel().x));

                    drivetrain.setVel({ YLimiter.getVelocity(), RZLimiter.getVelocity(), XLimiter.getVelocity() });
                }
            }
        }

        drivetrain.run();
    }

    /**
     * @brief  make drivetrain approach set velocity following acceleration limits
     * @param  _vel: (JTwoDTransform) target velocity
     * @param  _run: (bool) default=false, true=run gets called within this function, false=call run yourself outside this function
     */
    void moveVel(JTwoDTransform _vel, bool _run = false)
    {
        if (distMode || !controlled) {
            JTwoDTransform vel = getVel();
            YLimiter.setVelocity(vel.y);
            RZLimiter.setVelocity(vel.rz);
            XLimiter.setVelocity(vel.x);
        }

        controlled = true;
        distMode = false;

        velTarget = _vel;

        if (_run)
            run();
    }

    /**
     * @brief  make each axis of the drivetrain go to target position, following accel and vel limits
     * @note  each axis is controlled separately, you are not setting absolute position, just target distance for each axis
     * @param  _dist: (JTwoDTransform) target distance
     * @param  _run: (bool) default=false, true=run gets called within this function, false=call run yourself outside this function
     */
    void moveDist(JTwoDTransform _dist, bool _run = false)
    {
        if (!distMode || !controlled) {
            YLimiter.resetTime();
            XLimiter.resetTime();
            RZLimiter.resetTime();
            JTwoDTransform vel = getVel();
            YLimiter.setVelocity(vel.y);
            RZLimiter.setVelocity(vel.rz);
            XLimiter.setVelocity(vel.x);
            JTwoDTransform dist = getDist();
            YLimiter.setPosition(dist.y);
            RZLimiter.setPosition(dist.rz);
            XLimiter.setPosition(dist.x);
        }
        YLimiter.setTarget(_dist.y);
        RZLimiter.setTarget(_dist.rz);
        XLimiter.setTarget(_dist.x);

        controlled = true;
        distMode = true;

        if (_run)
            run();
    }

    void moveDistY(float _y, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.y = _y;
        moveDist(targ, _run);
    }

    void moveDistRZ(float _rz, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.rz = _rz;
        moveDist(targ, _run);
    }

    void moveDistX(float _x, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.x = _x;
        moveDist(targ, _run);
    }

    void moveDistInc(JTwoDTransform _d, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ += _d;
        moveDist(targ, _run);
    }

    void moveDistYInc(float _y, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.y += _y;
        moveDist(targ, _run);
    }

    void moveDistRZInc(float _rz, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.rz += _rz;
        moveDist(targ, _run);
    }

    void moveDistXInc(float _x, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.x += _x;
        moveDist(targ, _run);
    }

    bool isVelZero()
    {
        JTwoDTransform vel = getVel(false);
        return (vel.x == 0 && vel.y == 0 && vel.rz == 0);
    }

    /**
     * @brief  true if going to distance target, false if velocity based
     * @retval bool
     */
    bool getDistMode()
    {
        return distMode;
    }

    /**
     * @brief  true=velocity and acceleration is limited, false=writing directly to drivetrain
     * @retval (bool)
     */
    bool getIsControlled()
    {
        return controlled;
    }

    JTwoDTransform getDistTarget()
    {
        if (controlled && distMode) {
            return { YLimiter.getTarget(), RZLimiter.getTarget(), XLimiter.getTarget() };
        } else {
            return getDist();
        }
    }

    void setVelLimit(JTwoDTransform _velLim)
    {
        setVelLimitY(_velLim.y);
        setVelLimitX(_velLim.x);
        setVelLimitRZ(_velLim.rz);
    }
    void setAccelLimit(JTwoDTransform _accelLim)
    {
        setAccelLimitY(_accelLim.y);
        setAccelLimitX(_accelLim.x);
        setAccelLimitRZ(_accelLim.rz);
    }

    JTwoDTransform getVelLimit()
    {
        return { YLimiter.getVelLimit(), RZLimiter.getVelLimit(), XLimiter.getVelLimit() };
    }

    JTwoDTransform getAccelLimit()
    {
        return { YLimiter.getAccelLimit(), RZLimiter.getAccelLimit(), XLimiter.getAccelLimit() };
    }

    void setVelLimitY(float l)
    {
        YLimiter.setVelLimit(l);
    }
    void setVelLimitX(float l)
    {
        XLimiter.setVelLimit(l);
    }
    void setVelLimitRZ(float l)
    {
        RZLimiter.setVelLimit(l);
    }
    void setAccelLimitY(float l)
    {
        YLimiter.setAccelLimit(l);
    }
    void setAccelLimitX(float l)
    {
        XLimiter.setAccelLimit(l);
    }
    void setAccelLimitRZ(float l)
    {
        RZLimiter.setAccelLimit(l);
    }

    ////JDrivetrain methods:
    void setVel(JTwoDTransform _vel, bool _run = false)
    {
        controlled = false;
        drivetrain.setVel(_vel, false);
        if (_run)
            run();
    }
    void setDistSetpoint(JTwoDTransform _dist, bool _run = false)
    {
        controlled = false;
        drivetrain.setDistSetpoint(_dist, false);
        if (_run)
            run();
    }
    void setDistDelta(JTwoDTransform _dist, bool _run = false)
    {
        controlled = false;
        drivetrain.setDistDelta(_dist, false);
        if (_run)
            run();
    }
    void resetDist()
    {
        JTwoDTransform dist = getDist();
        JTwoDTransform offBy = { YLimiter.getTarget() - dist.y, RZLimiter.getTarget() - dist.rz, XLimiter.getTarget() - dist.x };

        YLimiter.setTarget(offBy.y);
        RZLimiter.setTarget(offBy.rz);
        XLimiter.setTarget(offBy.x);

        YLimiter.setPosition(YLimiter.getPosition() - dist.y);
        RZLimiter.setPosition(RZLimiter.getPosition() - dist.rz);
        XLimiter.setPosition(XLimiter.getPosition() - dist.x);

        drivetrain.resetDist();
    }
    JTwoDTransform getVel(bool _run = false)
    {
        if (_run)
            run();
        return drivetrain.getVel(false);
    }
    JTwoDTransform getDist(bool _run = false)
    {
        if (_run)
            run();
        return drivetrain.getDist(false);
    }
    /**
     * @brief returns what distances are being set as the setpoints for each axis of the drivetrain
     * @note only works in controlled dist mode  
     * @retval (JTwoDTransform)
     */
    JTwoDTransform getDistSetpoint()
    {
        return { YLimiter.getPosition(), RZLimiter.getPosition(), XLimiter.getPosition() };
    }
    JTwoDTransform getMaxVel()
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
        if (_enable && !getEnable()) {
            YLimiter.resetTime();
            RZLimiter.resetTime();
            XLimiter.resetTime();
            JTwoDTransform vel = getVel();
            YLimiter.setVelocity(vel.y);
            RZLimiter.setVelocity(vel.rz);
            XLimiter.setVelocity(vel.x);
            JTwoDTransform dist = getDist();
            YLimiter.setPosition(dist.y);
            RZLimiter.setPosition(dist.rz);
            XLimiter.setPosition(dist.x);
            drivetrain.setDistSetpoint(dist);
        }
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