#ifndef _J_DRIVETRAIN_TWO_SIDE_H
#define _J_DRIVETRAIN_TWO_SIDE_H
#include "JDrivetrain.h"
#include "JMotorController/JMotorController.h"
#include <Arduino.h>
/**
 * @brief  subclass of JDrivetrain for controlling a drivetrain with one JMotorController for each side of the drivetrain
 * @note   both sides should move forward when the controllers are given a positive speed
 */
class JDrivetrainTwoSide : public JDrivetrain {
public:
    JMotorController& left;
    JMotorController& right;
    float width;

    float yPos;
    float zRot;
    float xPos;

    float leftVel;
    float rightVel;
    float leftDist;
    float rightDist;

    JDrivetrainTwoSide(JMotorController& _left, JMotorController& _right, float _width)
        : left(_left)
        , right(_right)
    {
        yPos = 0;
        zRot = 0;
        xPos = 0;

        leftVel = 0;
        rightVel = 0;
        leftDist = 0;
        rightDist = 0;
    }
    bool setEnable(bool _enable)
    {
        return left.setEnable(_enable) || right.setEnable(_enable);
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
    twoDTransform getVel()
    {
        return { (leftVel + rightVel) / (float)2.0, (leftVel - rightVel) / width * (float)RAD_TO_DEG, 0 };
    }
    twoDTransform getPos()
    {
        return { yPos, zRot, xPos };
    }
    twoDTransform getDist()
    {
        return { (leftDist + rightDist) / (float)2.0, (leftDist - rightDist) / width * (float)RAD_TO_DEG, 0 };
    }
    void setVel(twoDTransform _vel)
    {
        float rotation = _vel.rz * width / RAD_TO_DEG / 2;
        left.setVel(_vel.y + rotation, false);
        right.setVel(_vel.y - rotation, false);
    }
    void setDistSetpoint(twoDTransform _dist)
    {
        float rotation = _dist.rz * width / RAD_TO_DEG / 2;
        left.setPosSetpoint(_dist.y + rotation, false);
        right.setPosSetpoint(_dist.y - rotation, false);
    }
    void setDistDelta(twoDTransform _dist)
    {
        float rotation = _dist.rz * width / RAD_TO_DEG / 2;
        left.setPosDelta(_dist.y + rotation, false);
        right.setPosDelta(_dist.y - rotation, false);
    }
    void resetDist()
    {
        left.resetPos();
        right.resetPos();
    }
    void resetPos(twoDTransform _pos)
    {
        yPos = _pos.y;
        zRot = _pos.rz;
        xPos = _pos.x;
    }
    twoDTransform getMaxVel()
    {
        return { min(left.getMaxVel(), right.getMaxVel()), (left.getMaxVel() - right.getMaxVel()) / width * (float)RAD_TO_DEG, 0 };
    }
};
#endif
