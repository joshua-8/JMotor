#ifndef JMOTOR_DRIVER_TMC7300_H
#define JMOTOR_DRIVER_TMC7300_H
#include "JMotorDriver.h"
#include <Arduino.h>
#include <TMC7300.h>
/**
 * @brief control a motor connected to a TMC7300 motor driver chip
 * @note make sure to call ic.begin() for each tmc7300 chip before enabling a motor driver.
 * https://github.com/joshua-8/TMC7300
 * https://www.analog.com/media/en/technical-documentation/data-sheets/TMC7300_datasheet_rev1.08.pdf
 *
 */
class JMotorDriverTMC7300 : public JMotorDriver {
protected:
    boolean enabled;
    boolean channel;

public:
    /**
     * @brief reference to TMC7300IC class, use ic.writeField() to change advanced settings
     * */
    TMC7300IC& ic;

public:
    /**
     * @brief control a motor connected to a TMC7300 dual motor driver chip
     * @param  _ic: an instance of the TMC7300IC class, for communicating with the motor driver chip
     * @param  _channel: 0 or 1, which motor?
     */
    JMotorDriverTMC7300(TMC7300IC& _ic, boolean _channel)
        : ic(_ic)
    {
        enabled = false;
        channel = _channel;
    }
    bool set(float val)
    {
        val = constrain(val, -1, 1);
        uint32_t value = (uint32_t)(val * 255) & 0b111111111;
        if (enabled) {
            if (!channel) {
                if (ic.readField(TMC7300_PWM_A, false) != (value >= 0 ? value : (value & 0xFF) | 1 << 9)) {
                    ic.writeField(TMC7300_PWM_A, value, true);
                }
            } else {
                if (ic.readField(TMC7300_PWM_B, false) != (value >= 0 ? value : (value & 0xFF) | 1 << 9)) {
                    ic.writeField(TMC7300_PWM_B, value, true);
                }
            }
        }
        return abs(val) < 1;
    }
    bool setEnable(bool _enable)
    {
        if (_enable == false) {
            if (!channel) {
                ic.writeField(TMC7300_PWM_A, 0);
            } else {
                ic.writeField(TMC7300_PWM_B, 0);
            }
        }
        if (_enable == true && enabled == false) {
            if (!channel) {
                ic.writeField(TMC7300_PWM_A, 0);
            } else {
                ic.writeField(TMC7300_PWM_B, 0);
            }
        }
        enabled = _enable;
        return true;
    }
    bool getEnable()
    {
        return enabled;
    }
    float getMaxRange()
    {
        return 1;
    }
    float getMinRange()
    {
        return -1;
    }
    bool enable()
    {
        return setEnable(true);
    }
    bool disable()
    {
        return setEnable(false);
    }
};

#endif // JMOTOR_DRIVER_TMC7300_H
