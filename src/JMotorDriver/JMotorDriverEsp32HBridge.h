#ifndef JMOTOR_DRIVER_ESP32HBRIDGE_H
#define JMOTOR_DRIVER_ESP32HBRIDGE_H
#include "JMotorDriver.h"
#include "JMotorDriver/JMotorDriverEsp32PWM.h"
#include <Arduino.h>
class JMotorDriverEsp32HBridge : public JMotorDriver {
private:
    bool enabled = false;
    bool invertSignals = false;

public:
    JMotorDriverEsp32PWM pwmDriverPos;
    JMotorDriverEsp32PWM pwmDriverNeg;
    bool reverse;
    // TODO: COMMENT
    JMotorDriverEsp32HBridge(byte _pinPosCh, byte _pinPos, byte _pinNeg, byte _pinNegCh, bool _reverse = false, int _freq = 2000, int _resolution = 12, bool _invertSignals = false)
        : pwmDriverPos { _pinPosCh, _pinPos, _freq, _resolution, LOW }
        , pwmDriverNeg { _pinNegCh, _pinNeg, _freq, _resolution, LOW }
    {
        enabled = false;
        reverse = _reverse;
        invertSignals = _invertSignals;
    }
    bool set(float val)
    {
        if (reverse) {
            val = -val;
        }
        if (enabled) {
            if (invertSignals == true) {
                float maxRangeN = pwmDriverNeg.getMaxRange();
                float maxRangeP = pwmDriverPos.getMaxRange();
                if (val > 0) {
                    pwmDriverNeg.set(maxRangeN);
                    pwmDriverPos.set(maxRangeP - val);
                } else if (val < 0) {
                    pwmDriverPos.set(maxRangeP);
                    pwmDriverNeg.set(maxRangeN - (-val));
                } else {
                    pwmDriverPos.set(maxRangeP);
                    pwmDriverNeg.set(maxRangeN);
                }
            } else {
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
#endif // JMOTOR_DRIVER_ESP32HBRIDGE_H
