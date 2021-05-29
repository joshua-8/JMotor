#ifndef J_DRIVETRAIN_CONTROLLER_BASIC_H
#define J_DRIVETRAIN_CONTROLLER_BASIC_H
#include "JDrivetrain/JDrivetrain.h"
#include <Arduino.h>
class JDrivetrainControllerBasic : public JDrivetrain {
protected:
    JDrivetrain& drivetrain;

public:
    JDrivetrainControllerBasic(JDrivetrain& _drivetrain)
        : drivetrain(_drivetrain)
    {
    }

    void run()
    {
        drivetrain.run();
    }

    /**
     * vel and accel for each axis
     * pause?
     * drive with turn radius (low priority)
     * set vels smoothed vel and accel
     * set smoothed dist targets (derivs limiter)
     * 
     */ 

    ////JDrivetrain methods:

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
    void setVel(twoDTransform _vel, bool _run = false)
    {
        drivetrain.setVel(_vel, false);
        if (_run)
            run();
    }
    void setDistSetpoint(twoDTransform _dist, bool _run = false)
    {
        drivetrain.setDistSetpoint(_dist, false);
        if (_run)
            run();
    }
    void setDistDelta(twoDTransform _dist, bool _run = false)
    {
        drivetrain.setDistDelta(_dist, false);
        if (_run)
            run();
    }
    void resetDist()
    {
        drivetrain.resetDist();
    }
    twoDTransform getMaxVel()
    {
        return drivetrain.getMaxVel();
    }
    void setMotorVels(float vels[], bool _run = false)
    {
        drivetrain.setMotorVels(vels, false);
        if (_run)
            run();
    }
    void setMotorDistSetpoints(float vels[], bool _run = false)
    {
        drivetrain.setMotorDistSetpoints(vels, false);
        if (_run)
            run();
    }
    void setMotorDistDeltas(float vels[], bool _run = false)
    {
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
};
#endif