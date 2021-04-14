/**
 * @brief  L293 motor driver chip: https://www.ti.com/lit/ds/symlink/l293.pdf
 * @note   platform: AVR (standard Arduinos)
 */
#ifndef J_MOTOR_DRIVER_AVR_L293_H
#define J_MOTOR_DRIVER_AVR_L293_H
#include "JMotorDriver.h"
#include "JMotorDriverAvrPWM.h"
class JMotorDriverAvrL293 : public JMotorDriver {
private:
    boolean enabled = false;
    int i1;
    int i2;

public:
    JMotorDriverAvrPWM pwmDriver;
    boolean breakOn; // can be changed while running
    /**
     * @brief  constructor, sets pins
     * @param  _en: enable(speed) pin on driver
     * @param  _i1: input pin 1 (direction)
     * @param  _i2: input pin 2 (direction)
     * @param  _breakOn = true: if true (default), add extra resistance to motor when set to 0 power (by shorting motor terminals)
     * @param  breakWhenDisabled = false: if false (default) turn off break when disabled, if true, keep electrically breaking
     */
    JMotorDriverAvrL293(int _en, int _i1, int _i2, boolean _breakOn = true, boolean breakWhenDisabled = false)
        : pwmDriver { _en, breakWhenDisabled }
    {
        enabled = false;
        i1 = _i1;
        i2 = _i2;
        breakOn = _breakOn;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::avrL293;
    }
    boolean set(float val)
    {
        if (enabled) {
            if (val > 0) {
                digitalWrite(i2, LOW);
                digitalWrite(i1, HIGH);
            } else if (val < 0) {
                digitalWrite(i1, LOW);
                digitalWrite(i2, HIGH);
            } else {
                digitalWrite(i1, HIGH);
                digitalWrite(i2, HIGH);
            }
            if (breakOn && val == 0) {
                pwmDriver.set(1); //activate break
            } else {
                pwmDriver.set(abs(val));
            }
        }
        return abs(val) < 1.0;
    }
    boolean setEnable(boolean enable)
    {
        if (enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, LOW);
                digitalWrite(i2, LOW);
                pwmDriver.setEnable(true);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, LOW);
                digitalWrite(i2, LOW);
                pwmDriver.setEnable(false);
                return true;
            }
        }
        return false;
    }
    boolean getEnable()
    {
        return enabled;
    }
    float getMaxRange()
    {
        return 1.0;
    }
    float getMinRange()
    {
        return -1.0;
    }
};

#endif
