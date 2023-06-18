#ifndef JSWERVE_ANGLE_CONTROL_LOOP_P_H
#define JSWERVE_ANGLE_CONTROL_LOOP_P_H
#include "JSwerveAngleControlLoop.h"
// TODO: ADD RANGE LIMITS
class JSwerveAngleControlLoopP : public JSwerveAngleControlLoop {
public:
    float kP = 0;
    // TODO: COMMENT
    JSwerveAngleControlLoopP(float _kP)
    {
        kP = _kP;
    }
    float calculate(float error)
    {
        return kP * error;
    }
};
#endif // JSWERVE_ANGLE_CONTROL_LOOP_P_H
