#ifndef J_CURVATURE_DRIVE_H
#define J_CURVATURE_DRIVE_H
#include "JTwoDTransform.h"
#include <Arduino.h>

/**
 * @brief  Cheezy Drive! https://frc1756-argos.github.io/ArgoBot-Drive-Training/tutorials/4/#c
 */
namespace JCurvatureDrive {
/**
 * @brief
 * @note   call with JCurvatureDrive::calculate()
 * @param  curvatureEnabled: true = use "cheezy" drive, false = pass values through with no changes
 * @param  input: (JTwoDTransform)
 * @param  minRadius: smaller = allow tighter turns in curvature mode
 * @param  reverseTurning: if true, positive turn is clockwise, if false, positive turn is counter clockwise
 * @retval output JTwoDTransform
 */
inline JTwoDTransform calculate(bool curvatureEnabled, JTwoDTransform input, float minRadius, bool reverseTurning)
{
    JTwoDTransform output = input;
    if (curvatureEnabled) {
        float speed = sqrt(sq(input.x) + sq(input.y));
        output.theta = output.theta * speed / minRadius;
    }

    return output;
}

} // end of namespace
#endif
