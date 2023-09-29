#ifndef JDRIVETRAIN_TWO_SIDE_H
#define JDRIVETRAIN_TWO_SIDE_H
#include "JDrivetrain.h"
#include "JMotorController/JMotorController.h"
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 * @brief  child class of JDrivetrain for controlling a drivetrain with one JMotorController for each side of the drivetrain
 * @note   both sides should move forward when the controllers are given a positive speed
 */
class JDrivetrainTwoSide : public JDrivetrain {
public:
    JMotorController& left;
    JMotorController& right;

protected:
    float width;
    float leftVel;
    float rightVel;
    float leftDist;
    float rightDist;

    JTwoDTransform vel;
    JTwoDTransform dist;
    JTwoDTransform maxVel;

public:
    JDrivetrainTwoSide(JMotorController& _left, JMotorController& _right, float _width)
        : left(_left)
        , right(_right)
    {
        width = _width;

        leftVel = 0;
        rightVel = 0;
        leftDist = 0;
        rightDist = 0;
    }
    void run()
    {
        left.run();
        right.run();

        leftVel = left.getVel();
        rightVel = right.getVel();
        leftDist = left.getPos();
        rightDist = right.getPos();

        vel = { (leftVel + rightVel) / (float)2.0, 0, (rightVel - leftVel) / width };
        dist = { (leftDist + rightDist) / (float)2.0, 0, (rightDist - leftDist) / width };
        maxVel = { min(left.getMaxVel(), right.getMaxVel()), 0, min(left.getMaxVel(), right.getMaxVel()) * (float)2.0 / width };
    }
    bool setEnable(bool _enable)
    {
        bool retl = left.setEnable(_enable);
        bool retr = right.setEnable(_enable);
        return retl || retr;
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
        return left.getEnable() || right.getEnable();
    }
    JTwoDTransform getVel(bool _run = false)
    {
        if (_run)
            run();
        return vel;
    }
    JTwoDTransform getDist(bool _run = false)
    {
        if (_run)
            run();
        return dist;
    }
    void setVel(JTwoDTransform _vel, bool _run = false)
    {
        float rotation = _vel.theta * width / 2;
        left.setVel(_vel.x - rotation, false);
        right.setVel(_vel.x + rotation, false);
        if (_run)
            run();
    }
    void setDistSetpoint(JTwoDTransform _dist, bool _run = false)
    {
        float rotation = _dist.theta * width / 2;
        left.setPosSetpoint(_dist.x - rotation, false);
        right.setPosSetpoint(_dist.x + rotation, false);
        if (_run)
            run();
    }
    void setDistDelta(JTwoDTransform _dist, bool _run = false)
    {
        float rotation = _dist.theta * width / 2;
        left.setPosDelta(_dist.x - rotation, false);
        right.setPosDelta(_dist.x + rotation, false);
        if (_run)
            run();
    }
    void resetDist()
    {
        dist = { 0, 0, 0 };
        left.resetPos();
        right.resetPos();
    }
    JTwoDTransform getMaxVel()
    {
        return maxVel;
    }
    float getMotorVel(unsigned char i)
    {
        if (i == 0)
            return leftVel;
        if (i == 1)
            return rightVel;
        return 0;
    }
    void setMotorVel(float vel, unsigned char i, bool _run = false)
    {
        if (i == 0)
            left.setVel(vel, _run);
        if (i == 1)
            right.setVel(vel, _run);
    }
    void setMotorDistSetpoint(float distSetpoint, unsigned char i, bool _run = false)
    {
        if (i == 0)
            left.setPosSetpoint(distSetpoint, _run);
        if (i == 1)
            right.setPosSetpoint(distSetpoint, _run);
    }
    void setMotorDistDelta(float distDelta, unsigned char i, bool _run = false)
    {
        if (i == 0)
            left.setPosDelta(distDelta, _run);
        if (i == 1)
            right.setPosDelta(distDelta, _run);
    }
    unsigned char getNumberMotors()
    {
        return 2;
    }

    float getWidth()
    {
        return width;
    }

    void setWidth(float _width)
    {
        width = _width;
    }
};
#endif // JDRIVETRAIN_TWO_SIDE_H
