#ifndef _J_DRIVETRAIN_H
#define _J_DRIVETRAIN_H
#include <Arduino.h>
class JDrivetrain {
public:
    /**
     * @brief  defines two dimensoinal transformation. y(forward/backwards), rz(clockwise/ccw), x(right/left)
     */
    struct twoDTransform {
        /**
         * @brief  backwards- | forward+
         */
        float y;
        /**
         * @brief  counterclockwise- | clockwise+
         */
        float rz;
        /**
         * @brief  left- | backwards+
         */
        float x;
    };
    virtual void run() = 0;
    virtual bool setEnable(bool _enable) = 0;
    virtual bool enable() = 0;
    virtual bool disable() = 0;
    virtual bool getEnable() = 0;
    virtual twoDTransform getVel(bool _run = false) = 0;
    virtual twoDTransform getPos(bool _run = false) = 0;
    virtual twoDTransform getDist(bool _run = false) = 0;
    virtual void setVel(twoDTransform _vel, bool _run = true) = 0;
    virtual void setDistSetpoint(twoDTransform _dist, bool _run = true) = 0;
    virtual void setDistDelta(twoDTransform _dist, bool _run = true) = 0;
    virtual void resetPos(twoDTransform _pos) = 0;
    virtual twoDTransform getMaxVel()=0;
};
#include "JDrivetrainTwoSide.h"
#endif
