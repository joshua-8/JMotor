#ifndef JSWERVE_ANGLE_CONTROL_LOOP_H
#define JSWERVE_ANGLE_CONTROL_LOOP_H
// TODO: COMMENTS
class JSwerveAngleControlLoop {
public:
    virtual float calculate(float error);
};
#include "JSwerveAngleControlLoopP.h"
#endif // JSWERVE_ANGLE_CONTROL_LOOP_H
