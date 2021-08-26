#ifndef _J_DRIVETRAIN_H
#define _J_DRIVETRAIN_H
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 * @brief  defines interface for controlling any ground-based drivetrain
 */
class JDrivetrain {
public:
    virtual void run() = 0;
    virtual bool setEnable(bool _enable) = 0;
    virtual bool enable() = 0;
    virtual bool disable() = 0;
    virtual bool getEnable() = 0;
    virtual JTwoDTransform getVel(bool _run = false) = 0;
    virtual JTwoDTransform getDist(bool _run = false) = 0;
    virtual void setVel(JTwoDTransform _vel, bool _run = true) = 0;
    virtual void setDistSetpoint(JTwoDTransform _dist, bool _run = true) = 0;
    virtual void setDistDelta(JTwoDTransform _dist, bool _run = true) = 0;
    virtual void resetDist() = 0;
    virtual JTwoDTransform getMaxVel() = 0;
    virtual void setMotorVels(float[], bool _run = false) = 0;
    virtual void setMotorDistSetpoints(float[], bool _run = false) = 0;
    virtual void setMotorDistDeltas(float[], bool _run = false) = 0;
    virtual float getMotorVel(unsigned char i) = 0;
    virtual unsigned char getNumberMotors() = 0;
};
#include "JDrivetrainTwoSide.h"
#endif
