#ifndef J_DRIVETRAIN_CONTROLLER_BASIC_H
#define J_DRIVETRAIN_CONTROLLER_BASIC_H
#include "JDrivetrain/JDrivetrain.h"
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 * @brief  basic drivetrain controller, supports smoothed movements on each axis
 * @note each axis is separate, this drivetrain controller doesn't combine movements over time to actually calculate its position
 */
// TODO: comments
class JDrivetrainControllerBasic : public JDrivetrain {
protected:
    bool controlled;
    bool distMode;
    unsigned long lastCalcMillis;

    JTwoDTransform velTarget;

    bool velNotPosDelta;

public:
    JDrivetrain& drivetrain;
    Derivs_Limiter XLimiter;
    Derivs_Limiter YLimiter;
    Derivs_Limiter ThetaLimiter;
    JTwoDTransform distError;
    JDrivetrainControllerBasic(JDrivetrain& _drivetrain, JTwoDTransform _velLimit, JTwoDTransform _accelLimit, JTwoDTransform _distError, bool _velNotPosDelta = true)
        : drivetrain(_drivetrain)
        , XLimiter(Derivs_Limiter(_velLimit.x, _accelLimit.x))
        , YLimiter(Derivs_Limiter(_velLimit.y, _accelLimit.y))
        , ThetaLimiter(Derivs_Limiter(_velLimit.theta, _accelLimit.theta))
        , distError(_distError)
    {
        YLimiter.setPreventGoingWrongWay(false);
        XLimiter.setPreventGoingWrongWay(false);
        ThetaLimiter.setPreventGoingWrongWay(false);
        YLimiter.setPreventGoingTooFast(true);
        ThetaLimiter.setPreventGoingTooFast(true);
        XLimiter.setPreventGoingTooFast(true);
        controlled = false;
        distMode = false;
        lastCalcMillis = 0;
        velNotPosDelta = _velNotPosDelta;
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
                if (distMode) { // go towards target position with smoothed velocity
                    YLimiter.calc();
                    ThetaLimiter.calc();
                    XLimiter.calc();

                    JTwoDTransform dist = getDist(); // prevent setpoint from getting to far from actual
                    YLimiter.setPosition(constrain(YLimiter.getPosition(), dist.y - distError.y, dist.y + distError.y));
                    ThetaLimiter.setPosition(constrain(ThetaLimiter.getPosition(), dist.theta - distError.theta, dist.theta + distError.theta));
                    XLimiter.setPosition(constrain(XLimiter.getPosition(), dist.x - distError.x, dist.x + distError.x));

                    drivetrain.setDistSetpoint({ XLimiter.getPosition(), YLimiter.getPosition(), ThetaLimiter.getPosition() }, false);
                } else {
                    // accelerate towards velocity target
                    if (YLimiter.getVelocity() != velTarget.y) {
                        YLimiter.setVelocity(YLimiter.getVelocity() + constrain(velTarget.y - YLimiter.getVelocity(), -YLimiter.getAccelLimit() * time, YLimiter.getAccelLimit() * time));
                    }
                    if (ThetaLimiter.getVelocity() != velTarget.theta) {
                        ThetaLimiter.setVelocity(ThetaLimiter.getVelocity() + constrain(velTarget.theta - ThetaLimiter.getVelocity(), -ThetaLimiter.getAccelLimit() * time, ThetaLimiter.getAccelLimit() * time));
                    }
                    if (XLimiter.getVelocity() != velTarget.x) {
                        XLimiter.setVelocity(XLimiter.getVelocity() + constrain(velTarget.x - XLimiter.getVelocity(), -XLimiter.getAccelLimit() * time, XLimiter.getAccelLimit() * time));
                    }

                    YLimiter.setVelocity(constrain(YLimiter.getVelocity(), -YLimiter.getVelLimit(), YLimiter.getVelLimit()));
                    ThetaLimiter.setVelocity(constrain(ThetaLimiter.getVelocity(), -ThetaLimiter.getVelLimit(), ThetaLimiter.getVelLimit()));
                    XLimiter.setVelocity(constrain(XLimiter.getVelocity(), -XLimiter.getVelLimit(), XLimiter.getVelLimit()));

                    YLimiter.setVelocity(constrain(YLimiter.getVelocity(), -getMaxVel().y, getMaxVel().y));
                    ThetaLimiter.setVelocity(constrain(ThetaLimiter.getVelocity(), -getMaxVel().theta, getMaxVel().theta));
                    XLimiter.setVelocity(constrain(XLimiter.getVelocity(), -getMaxVel().x, getMaxVel().x));

                    if (velNotPosDelta) {
                        drivetrain.setVel({ XLimiter.getVelocity(), YLimiter.getVelocity(), ThetaLimiter.getVelocity() });
                    } else {
                        drivetrain.setDistDelta({ XLimiter.getVelocity(), YLimiter.getVelocity(), ThetaLimiter.getVelocity() });
                    }
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
            ThetaLimiter.setVelocity(vel.theta);
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
            ThetaLimiter.resetTime();
            JTwoDTransform vel = getVel();
            YLimiter.setVelocity(vel.y);
            ThetaLimiter.setVelocity(vel.theta);
            XLimiter.setVelocity(vel.x);
            JTwoDTransform dist = getDist();
            YLimiter.setPosition(dist.y);
            ThetaLimiter.setPosition(dist.theta);
            XLimiter.setPosition(dist.x);
        }
        YLimiter.setTarget(_dist.y);
        ThetaLimiter.setTarget(_dist.theta);
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

    void moveDistTheta(float _theta, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.theta = _theta;
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

    void moveDistThetaInc(float _theta, bool _run = false)
    {
        JTwoDTransform targ = getDist();
        targ.theta += _theta;
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
        return (vel.x == 0 && vel.y == 0 && vel.theta == 0);
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
            return { XLimiter.getTarget(), YLimiter.getTarget(), ThetaLimiter.getTarget() };
        } else {
            return getDist();
        }
    }

    void setVelLimit(JTwoDTransform _velLim)
    {
        setVelLimitY(_velLim.y);
        setVelLimitX(_velLim.x);
        setVelLimitTheta(_velLim.theta);
    }
    void setAccelLimit(JTwoDTransform _accelLim)
    {
        setAccelLimitY(_accelLim.y);
        setAccelLimitX(_accelLim.x);
        setAccelLimitTheta(_accelLim.theta);
    }

    JTwoDTransform getVelLimit()
    {
        return { XLimiter.getVelLimit(), YLimiter.getVelLimit(), ThetaLimiter.getVelLimit() };
    }

    JTwoDTransform getAccelLimit()
    {
        return { XLimiter.getAccelLimit(), YLimiter.getAccelLimit(), ThetaLimiter.getAccelLimit() };
    }

    void setVelLimitY(float l)
    {
        YLimiter.setVelLimit(l);
    }
    void setVelLimitX(float l)
    {
        XLimiter.setVelLimit(l);
    }
    void setVelLimitTheta(float l)
    {
        ThetaLimiter.setVelLimit(l);
    }
    void setAccelLimitY(float l)
    {
        YLimiter.setAccelLimit(l);
    }
    void setAccelLimitX(float l)
    {
        XLimiter.setAccelLimit(l);
    }
    void setAccelLimitTheta(float l)
    {
        ThetaLimiter.setAccelLimit(l);
    }

    /**
     * @brief true = uses setVel() when setting drivetrain velocity, false = uses setPosDelta() when setting drivetrain velocity
     * @param  _velNotPosDelta: (bool)
     */
    void setVelNotPosDelta(bool _velNotPosDelta)
    {
        velNotPosDelta = _velNotPosDelta;
    }

    ////JDrivetrain methods:
    /**
     * @brief  sets drivetrain velocity immediately
     * @param  _vel: JTwoDTransform of velocity wanted for drivetrain
     * @param  _run: (bool) default=false, true=run gets called within this function, false=call run yourself outside this function
     * @retval None
     */
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
        JTwoDTransform offBy = { XLimiter.getTarget() - dist.x, YLimiter.getTarget() - dist.y, ThetaLimiter.getTarget() - dist.theta };

        YLimiter.setTarget(offBy.y);
        ThetaLimiter.setTarget(offBy.theta);
        XLimiter.setTarget(offBy.x);

        YLimiter.setPosition(YLimiter.getPosition() - dist.y);
        ThetaLimiter.setPosition(ThetaLimiter.getPosition() - dist.theta);
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
        return { XLimiter.getPosition(), YLimiter.getPosition(), ThetaLimiter.getPosition() };
    }
    JTwoDTransform getMaxVel()
    {
        return drivetrain.getMaxVel();
    }
    float getMotorVel(unsigned char i)
    {
        return drivetrain.getMotorVel(i);
    }
    void setMotorVel(float vel, unsigned char i, bool _run = false)
    {
        drivetrain.setMotorVel(vel, i, _run);
    }
    void setMotorDistSetpoint(float distSetpoint, unsigned char i, bool _run = false)
    {
        drivetrain.setMotorDistSetpoint(distSetpoint, i, _run);
    }
    void setMotorDistDelta(float distDelta, unsigned char i, bool _run = false)
    {
        drivetrain.setMotorDistDelta(distDelta, i, _run);
    }
    unsigned char getNumberMotors()
    {
        return drivetrain.getNumberMotors();
    }

    bool setEnable(bool _enable)
    {
        if (_enable && !getEnable()) {
            YLimiter.resetTime();
            ThetaLimiter.resetTime();
            XLimiter.resetTime();
            JTwoDTransform vel = getVel();
            YLimiter.setVelocity(vel.y);
            ThetaLimiter.setVelocity(vel.theta);
            XLimiter.setVelocity(vel.x);
            JTwoDTransform dist = getDist();
            YLimiter.setPosition(dist.y);
            ThetaLimiter.setPosition(dist.theta);
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