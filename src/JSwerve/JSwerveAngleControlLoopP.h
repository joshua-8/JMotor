#ifndef JSWERVE_ANGLE_CONTROL_LOOP_P_H
#define JSWERVE_ANGLE_CONTROL_LOOP_P_H
#include "JSwerveAngleControlLoop.h"
class JSwerveAngleControlLoopP : public JSwerveAngleControlLoop {
public:
    float kP = 0;
    float range = 0;
    // TODO: COMMENT
    JSwerveAngleControlLoopP(float _kP, float _range)
    {
        kP = _kP;
        range = _range;
    }
    float calculate(float error)
    {
        return constrain(kP * error, -range, range);
    }
};
#endif // JSWERVE_ANGLE_CONTROL_LOOP_P_H
