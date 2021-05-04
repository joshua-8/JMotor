#ifndef J_MOTOR_COMPENSATOR_H
#define J_MOTOR_COMPENSATOR_H
#include "JVoltageCompensator.h"
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for converting from speed to driver input.
 * It should compensate for factors like motor response and supply voltage.
 **/
class JMotorCompensator {
public:
    /**
     * @brief  calculate
     * @param  val: (float) speed
     * @retval (float) value to set driver
     */
    virtual float compensate(float val);

    /**
     * @brief  How fast of a motor speed setting would get adjusted to full motor power
     * @note   may change as battery voltage changes
     * @retval  (float)
     */
    virtual float getMaxVel();
};
#include "JMotorCompDirect.h"
#include "JMotorCompStandard.h"
#endif
