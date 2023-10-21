#ifndef JDRIVETRAIN_FIELD_ORIENTED_H
#define JDRIVETRAIN_FIELD_ORIENTED_H
#include "JDrivetrain/JDrivetrain.h"
#include "JTwoDTransform.h"
#include <Arduino.h>
/**
 /// TODO: NOT DONE YET*
* //TODO: COMMENT
*/
// void run()
// {
//     JTwoDTransform velocity = { 0, 0, 0 };
//     velocity.theta = vel.theta;
//     velocity.x = vel.x * cos(-theta) - vel.y * sin(-theta);
//     velocity.y = vel.x * sin(-theta) + vel.y * cos(-theta);
//     drivetrain.setVel(velocity);
// }
// /**
//  * @brief  for field-oriented mode to work, the imu heading, or an inaccurate encoder calculated heading must be given
//  * @param  _heading: (float) degrees
//  */
// void giveImuReading(float _heading)
// {
//     imuHeading = _heading - imuHeadingOffset;
//     if (imuHeading - lastImuHeading > 180)
//         fullRotations--;
//     else if (imuHeading - lastImuHeading < -180)
//         fullRotations++;
//     lastImuHeading = imuHeading;
//     theta = imuHeading + fullRotations * 360;
// }

/**
 * @brief  interface for a drivetrain that makes it into field oriented
 * @note   only makes sense for drivetrains that can move in any direction
 */
class JDrivetrainFieldOriented : public JDrivetrain {
protected:
    JDrivetrain& drivetrain;
    float imuHeading = 0;
    float imuHeadingOffset = 0;
    float lastImuHeading = 0;
    float theta = 0;
    int fullRotations = 0;
    JTwoDTransform dist = { 0, 0, 0 };
    JTwoDTransform lastDrivetrainDist = { 0, 0, 0 };

public:
    JDrivetrainFieldOriented(JDrivetrain& _drivetrain)
        : drivetrain(_drivetrain)
    {
    }
    float getHeading()
    {
        return theta;
    }
    void giveHeading(float _heading)
    {
        imuHeading = _heading - imuHeadingOffset;
        if (imuHeading - lastImuHeading > PI)
            fullRotations--;
        else if (imuHeading - lastImuHeading < -PI)
            fullRotations++;
        lastImuHeading = imuHeading;
        theta = imuHeading + fullRotations * TWO_PI;
    }
    void run()
    {
        drivetrain.run();
        JTwoDTransform drivetrainMovement = drivetrain.getDist(false) - lastDrivetrainDist;
        dist += JTwoDTransform::rotate(drivetrainMovement, theta);
        theta += drivetrainMovement.theta;
        lastDrivetrainDist = drivetrain.getDist(false);
    }
    bool setEnable(bool _enable)
    {
        return drivetrain.setEnable(_enable);
    }
    bool enable()
    {
        return drivetrain.enable();
    }
    bool disable()
    {
        return drivetrain.disable();
    }
    bool getEnable()
    {
        return drivetrain.getEnable();
    }
    void setVel(JTwoDTransform _vel, bool _run = false)
    {
        drivetrain.setVel(JTwoDTransform::rotate(_vel, theta), _run);
    }
    JTwoDTransform getMaxVel()
    {
        return drivetrain.getMaxVel();
    }
    JTwoDTransform getVel(bool _run = false)
    {
        JTwoDTransform _vel = drivetrain.getVel(_run);
        return JTwoDTransform::rotate(_vel, theta);
    }
    JTwoDTransform getDist(bool _run = false)
    {
        if (_run)
            run();
        return dist;
    }
    /**
     * @brief
     * @note   don't set a dist setpoint far from the current distance
     * @param  _dist:
     * @param  _run:
     * @retval None
     */
    void setDistSetpoint(JTwoDTransform _dist, bool _run = false)
    {
        JTwoDTransform dist = _dist - getDist(_run);
        setDistDelta(dist, _run);
    }
    void setDistDelta(JTwoDTransform _dist, bool _run = false)
    {
        drivetrain.setDistDelta(JTwoDTransform::rotate(_dist, theta), _run);
    }
    void resetDist()
    {
        drivetrain.resetDist();
        dist = { 0, 0, 0 };
        lastDrivetrainDist = { 0, 0, 0 };
        fullRotations = 0;
        theta = 0;
    }
    void setMotorVel(float vel, unsigned char i, bool _run = false)
    {
        drivetrain.setMotorVel(vel, i, _run);
    }
    float getMotorVel(unsigned char i)
    {
        return drivetrain.getMotorVel(i);
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
};

#endif // JDRIVETRAIN_FIELD_ORIENTED_H
