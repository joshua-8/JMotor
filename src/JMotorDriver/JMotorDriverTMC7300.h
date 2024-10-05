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
    boolean checkDriver;
    unsigned long lastCheckedIC;
    byte lastCheckedICCounter;

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
     * @param  _checkDriver: true to check driver settings and reset if needed (helps recover from power cycling the driver but not the microcontroller)
     */
    JMotorDriverTMC7300(TMC7300IC& _ic, boolean _channel, boolean _checkDriver = true)
        : ic(_ic)
    {
        enabled = false;
        channel = _channel;
        checkDriver = _checkDriver;
        lastCheckedIC = 0;
        lastCheckedICCounter = ic.getChipAddress() + 4 * channel;
    }
    bool set(float val)
    {
        if (checkDriver) {
            if (millis() - lastCheckedIC > 125) { // attempt to spread out driver checks across the 4 drivers that might be connected
                lastCheckedIC = millis();
                lastCheckedICCounter++;
                if (lastCheckedICCounter == 8) {
                    ic.checkDriver();
                    lastCheckedICCounter = 0;
                    Serial.printf("checked %d %d %d\n", channel, ic.getChipAddress(), millis());
                }
            }
        }
        val = constrain(val, -1, 1);
        uint32_t value = (uint32_t)(val * 255) & 0b111111111;
        if (enabled) {
            if (!channel) {
                if (ic.readField(TMC7300_PWM_A, false) != value) {
                    ic.writeField(TMC7300_PWM_A, value, true);
                }
            } else {
                if (ic.readField(TMC7300_PWM_B, false) != value) {
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
