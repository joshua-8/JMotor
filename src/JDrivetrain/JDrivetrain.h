#ifndef JDRIVETRAIN_H
#define JDRIVETRAIN_H
#include "JDrivetrainBasic.h"
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 * @brief  defines interface for controlling any ground-based drivetrain
 */
// TODO: comments
class JDrivetrain : public JDrivetrainBasic {
public:
    virtual JTwoDTransform getVel(bool _run = false);
    virtual JTwoDTransform getDist(bool _run = false);
    virtual void setDistSetpoint(JTwoDTransform _dist, bool _run = false);
    virtual void setDistDelta(JTwoDTransform _dist, bool _run = false);
    virtual void resetDist();
    virtual void setMotorVel(float vel, unsigned char i, bool _run = false);
    virtual void setMotorDistSetpoint(float distSetpoint, unsigned char i, bool _run = false);
    virtual void setMotorDistDelta(float distDelta, unsigned char i, bool _run = false);
    virtual float getMotorVel(unsigned char i);
    virtual unsigned char getNumberMotors();
};
#include "JDrivetrainFieldOriented.h"
#include "JDrivetrainSwerve.h"
#include "JDrivetrainTwoSide.h"
#endif // JDRIVETRAIN_H
