#ifndef J_DEADZONE_REMOVER_H
#define J_DEADZONE_REMOVER_H
#include "JTwoDTransform.h"
#include <Arduino.h>
namespace JDeadzoneRemover {

/**
 * @brief  If a motor's minimum speed is high, you can use this function to reduce the deadzone where nothing moves.
 *
 * example usage:
 * ` value = JDeadzoneRemover::calculate(value, myController.getMinVel(), myController.getMaxVel(), .01); `
 *
 * @note   call with JDeadzoneRemover::calculate()
 * @param  value: (float) input value
 * @param  start: (float) when value goes outside deadzone, output this
 * @param  max: (float) when value=1, output this
 * @param  deadzone: (float, default=0) deadzone for the input value (if your joystick might not center perfectly)
 * @retval  (float) output value
 */
inline float calculate(float value, float start, float max, float deadzone = 0)
{
    value = constrain(value, -1.0, 1.0);
    if (abs(value) <= abs(deadzone)) {
        return 0;
    }
    if (value > deadzone) {
        // return floatMap(value, deadzone, 1.0, start, max);
        return (value - deadzone) * (max - start) / (1.0 - deadzone) + start;
    }
    if (value < -deadzone) {
        // return floatMap(value, -deadzone, -1.0, -start, -max);
        return (value + deadzone) * (-max + start) / (-1.0 + deadzone) - start;
    }
    return 0;
}

/**
 * @brief  If a motor's minimum speed is high, you can use this function to reduce the deadzone where nothing moves.
 *
 * example usage:
 * ` value = JDeadzoneRemover::calculate(value, myController.getMinVel(), myController.getMaxVel(), .01); `
 *
 * @note   call with JDeadzoneRemover::calculate()
 * @param  value: input value (constrained internally between -1 and 1)
 * @param  start: when value goes outside deadzone, output this
 * @param  max: when value=1, output this
 * @param  deadzone: (default=0) deadzone for the input value (if your joystick might not center perfectly)
 * @retval  output value
 */
inline JTwoDTransform calculate(JTwoDTransform value, JTwoDTransform start, JTwoDTransform max, JTwoDTransform deadzone = { 0, 0, 0 })
{
    JTwoDTransform output = { 0, 0, 0 };
    for (int i = 0; i < 3; i++) {
        float currentValue = constrain(value.getByNumber(i), -1.0, 1.0);
        float currentDeadzone = deadzone.getByNumber(i);
        float currentStart = start.getByNumber(i);
        float currentMax = max.getByNumber(i);
        float ret = 0;
        if (abs(currentValue) <= abs(currentDeadzone)) {
            ret = 0;
        }
        if (currentValue > currentDeadzone) {
            // return floatMap(value, deadzone, 1.0, start, max);
            ret = (currentValue - currentDeadzone) * (currentMax - currentStart) / (1.0 - currentDeadzone) + currentStart;
        }
        if (currentValue < -currentDeadzone) {
            // return floatMap(value, -deadzone, -1.0, -start, -max);
            ret = (currentValue + currentDeadzone) * (-currentMax + currentStart) / (-1.0 + currentDeadzone) - currentStart;
        }
        output.setByNumber(i, ret);
    }
    return output;
}

}; // end of namespace
#endif
