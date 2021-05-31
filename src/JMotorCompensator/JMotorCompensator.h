#ifndef J_MOTOR_COMPENSATOR_H
#define J_MOTOR_COMPENSATOR_H
#include "JVoltageCompensator.h"
#include <Arduino.h>
/**
 * @brief  This class defines a common interface for converting from speed to driver input.
 * It should compensate for factors like motor response and supply voltage.
 **/
class JMotorCompensator {
protected:
    float driverRange = 1.0;
    float maxDriverRange = 1.0;

public:
    /**
     * @brief  set what value makes the driver output 100% of supply voltage
     * @param  _driverRange: (float) max range of driver
     */
    void setDriverRange(float _driverRange)
    {
        driverRange = _driverRange;
    }
    /**
     * @brief  set what fraction of driverRange can be used
     * @param  _driverRangeAmount: (float)
     */
    void setMaxDriverRangeAmount(float _driverRangeAmount)
    {
        maxDriverRange = max(_driverRangeAmount, (float)0.0);
    }
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

    /**
     * @brief  Slowest speed motor can go
     * @retval  (float)
     */
    virtual float getMinVel();

    /**
     * @brief  get what value makes the driver output 100% of supply voltage
     * @retval (float)
     */
    float getDriverRange()
    {
        return driverRange;
    }

    /**
     * @brief  get what fraction of driverRange can be used
     * @retval (float)
     */
    float getMaxDriverRangeAmount()
    {
        return maxDriverRange;
    }
};
#include "JMotorCompBasic.h"
#include "JMotorCompDirect.h"
#include "JMotorCompStandard.h"
#endif
