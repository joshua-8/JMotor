#ifndef JDRIVETRAIN_SWERVE_H
#define JDRIVETRAIN_SWERVE_H
#include "JDrivetrainBasic.h"
#include "JSwerve/JSwerveModule.h"
#include "JTwoDTransform.h"
#include <Arduino.h>
// TODO: COMMENT
/**
 * @brief  number of swerve modules
 */
template <byte N>
/**
 * @brief  swerve drivetrain.
 * @note not actually a subclass of JDrivetrain, since currently only velocity control is implemented
 * The code for calculating speed and angles for each wheel is in this file.
 */
class JDrivetrainSwerve : public JDrivetrainBasic {
public:
    JSwerveModule* swerveModule[N];
    float modPosL[N]; // left right position of swerve modules
    float modPosF[N]; // forwards backwards position of swerve modules
    float targetTheta[N];
    float targetSpeed[N];
    float deadzone = 0.0001;
    float centerL = 0;
    float centerF = 0;

protected:
public:
    /**
     * @brief
     * @note
     * @param  _swerveModule[N]: array of pointers to swerveModules. Sorry, couldn't figure out how to hide the pointers from you. example: pass swerveModules to constructor: JSwerveModule* swerveModules[3] = { &swerveModule1, &swerveModule2, &swerveModule3 }; where swerveModuleN is a subclass of JSwerveModule

     * @param  _modPosForwards[N]:
     * @param  _modPosLeft[N]:
     * @param  _centerL:
     * @param  _centerF:
     * @retval
     */
    JDrivetrainSwerve(JSwerveModule* _swerveModule[N], float _modPosForwards[N], float _modPosLeft[N], float _centerL = 0, float _centerF = 0)
    {
        centerL = _centerL;
        centerF = _centerF;
        for (byte i = 0; i < N; i++) {
            swerveModule[i] = _swerveModule[i];
            modPosL[i] = _modPosLeft[i];
            modPosF[i] = _modPosForwards[i];
        }
    }
    void setCenter(float forwards, float left)
    {
        centerF = forwards;
        centerL = left;
    }
    void run()
    {
        for (byte i = 0; i < N; i++) {
            swerveModule[i]->run();
        }
    }

    bool setEnable(bool _enable)
    {
        bool ret = false;
        for (byte i = 0; i < N; i++) {
            ret = swerveModule[i]->setEnable(_enable) || ret;
        }
        return ret;
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
        bool ret = true;
        for (byte i = 0; i < N; i++) {
            ret = swerveModule[i]->getEnable() && ret;
        }
        return ret;
    }

    void setVel(JTwoDTransform _vel, bool _run = true)
    {
        if (_vel.sumAbs() <= deadzone) { // robot not told to move? angle wheels to lock in place.
            for (byte i = 0; i < N; i++) {
                targetSpeed[i] = 0;
                targetTheta[i] = calcClosestAngle(degrees(atan2(modPosF[i] - centerF, modPosL[i] - centerL)), swerveModule[i]->getAngle());
            }
        } else { // swerve
            for (byte i = 0; i < N; i++) {
                float distance = sqrt(sq(modPosL[i] - centerL) + sq(modPosF[i] - centerF));
                float vx = _vel.y + _vel.theta * (modPosF[i] - centerF) / distance;
                float vy = _vel.x + _vel.theta * (modPosL[i] - centerL) / distance;
                if (abs(vx) + abs(vy) > deadzone) { // don't change direction if speed is tiny
                    float targetThetaHeading = degrees(atan2(vx, vy));
                    targetTheta[i] = calcClosestAngle(targetThetaHeading, swerveModule[i]->getAngle());
                    targetSpeed[i] = sqrt(sq(vx) + sq(vy)) * cos(radians(swerveModule[i]->getAngle() - targetThetaHeading));
                }
            }
        }
        float overspeed = 1; // module's target speed / module's max speed
        for (byte i = 0; i < N; i++) { // find highest overspeed
            overspeed = max(overspeed, abs(targetSpeed[i]) / swerveModule[i]->getMaxSpeed());
        }
        for (byte i = 0; i < N; i++) { // adjust target speed (post scaling)
            targetSpeed[i] = targetSpeed[i] / overspeed;
        }
        for (byte i = 0; i < N; i++) { // set modules to calculated speeds and angles
            swerveModule[i]->set(targetSpeed[i], targetTheta[i], false);
        }
    }
    static float calcClosestAngle(float target, float theta)
    {
        return fmod(fmod((target - theta + 90), 180) + 360, 180) - 90 + theta;
    }
};

#endif // JDRIVETRAIN_SWERVE_H
