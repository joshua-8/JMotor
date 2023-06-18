#ifndef JMOTOR_DRIVER_AVR_HBRIDGE_H
#define JMOTOR_DRIVER_AVR_HBRIDGE_H
#include "JMotorDriver.h"
#include "JMotorDriver/JMotorDriverAvrPWM.h"
#include <Arduino.h>
class JMotorDriverAvrHBridge : public JMotorDriver {
private:
    bool enabled = false;

public:
    JMotorDriverAvrPWM pwmDriverPos;
    JMotorDriverAvrPWM pwmDriverNeg;
    bool reverse;
    // TODO: COMMENT
    JMotorDriverAvrHBridge(byte _pinPos, byte _pinNeg, bool _reverse = false)
        : pwmDriverPos { _pinPos, LOW }
        , pwmDriverNeg { _pinNeg, LOW }
    {
        enabled = false;
        reverse = _reverse;
    }
    bool set(float val)
    {
        if (reverse) {
            val = -val;
        }
        if (enabled) {
            if (val > 0) {
                pwmDriverNeg.set(0);
                pwmDriverPos.set(val);
            } else if (val < 0) {
                pwmDriverPos.set(0);
                pwmDriverNeg.set(-val);
            } else {
                pwmDriverPos.set(0);
                pwmDriverNeg.set(0);
            }
        }
        return abs(val) < 1.0;
    }
    bool setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                // actually enable
                enabled = true;
                pwmDriverPos.setEnable(true);
                pwmDriverNeg.setEnable(true);
                return true;
            }
        } else { // disable
            if (enabled) {
                // actually disable
                enabled = false;
                pwmDriverPos.setEnable(false);
                pwmDriverNeg.setEnable(false);
                return true;
            }
        }
        return false;
    }
    bool getEnable()
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
#endif // JMOTOR_DRIVER_AVR_HBRIDGE_H
