#ifndef _J_DRIVETRAIN_TWO_SIDE_H
#define _J_DRIVETRAIN_TWO_SIDE_H
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

        vel = { (leftVel + rightVel) / (float)2.0, (leftVel - rightVel) / width * (float)RAD_TO_DEG, 0 };
        dist = { (leftDist + rightDist) / (float)2.0, (leftDist - rightDist) / width * (float)RAD_TO_DEG, 0 };
        maxVel = { min(left.getMaxVel(), right.getMaxVel()), min(left.getMaxVel(), right.getMaxVel()) * (float)2.0 / width * (float)RAD_TO_DEG, 0 };
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
        float rotation = _vel.rz * width / RAD_TO_DEG / 2;
        left.setVel(_vel.y + rotation, false);
        right.setVel(_vel.y - rotation, false);
        if (_run)
            run();
    }
    void setDistSetpoint(JTwoDTransform _dist, bool _run = false)
    {
        float rotation = _dist.rz * width / RAD_TO_DEG / 2;
        left.setPosSetpoint(_dist.y + rotation, false);
        right.setPosSetpoint(_dist.y - rotation, false);
        if (_run)
            run();
    }
    void setDistDelta(JTwoDTransform _dist, bool _run = false)
    {
        float rotation = _dist.rz * width / RAD_TO_DEG / 2;
        left.setPosDelta(_dist.y + rotation, false);
        right.setPosDelta(_dist.y - rotation, false);
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
    void setMotorVels(float vels[2], bool _run = false)
    {
        left.setVel(vels[0]);
        right.setVel(vels[1]);
        if (_run)
            run();
    }
    void setMotorDistSetpoints(float vels[2], bool _run = false)
    {
        left.setPosSetpoint(vels[0]);
        right.setPosSetpoint(vels[1]);
        if (_run)
            run();
    }
    void setMotorDistDeltas(float vels[2], bool _run = false)
    {
        left.setPosDelta(vels[0]);
        right.setPosDelta(vels[1]);
        if (_run)
            run();
    }
    float getMotorVel(unsigned char i)
    {
        if (i == 0)
            return leftVel;
        if (i == 1)
            return rightVel;
        return 0;
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
#endif
