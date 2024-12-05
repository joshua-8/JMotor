#ifndef JMOTOR_DRIVER_PCA9685HBRIDGE_H
#define JMOTOR_DRIVER_PCA9685HBRIDGE_H

#include "JMotorDriver.h"
#include <Arduino.h>
#include <PCA9685.h>
/**
 * @brief  Controls an H-bridge motor driver controlled by a PCA9685
 * Tested on the Alfredo Systems NoU3 https://github.com/AlfredoSystems/Alfredo-NoU3
 */
class JMotorDriverPCA9685HBridge : public JMotorDriver {
protected:
    bool enabled = false;
    bool reverse;
    bool breakWhenEnabled;
    bool breakWhenDisabled;
    byte channelPos;
    byte channelNeg;
    PCA9685& pca9685;

public:
    /**
     * @brief  Controls an H-bridge motor driver controlled by a PCA9685
     * @note  The PCA9685 must be initialized before this class is used to control a motor
     * @param  _pca9685: reference to an instance of janelia-arduino/PCA9685
     * @param  _channelPos: PCA channel that makes the motor go forward
     * @param  _channelNeg: PCA channel that makes the motor go backward
     * @param  _reverse: (bool, default=false) set to true if the motor is wired backwards
     * @param  _breakWhenEnabled: (bool, default=true) set to true to turn on electrical break mode when motor is enabled and speed is 0
     * @param  _breakWhenDisabled: (bool, default=false) set to true to turn on electrical break mode when motor is disabled
        example PCA9685 setup:
        Wire1.begin(PIN_I2C_SDA_IMU, PIN_I2C_SCL_IMU, 400000);
        pca9685.setupSingleDevice(Wire1, 0x40);
        pca9685.setupOutputEnablePin(12);
        pca9685.enableOutputs(12);
        pca9685.setToFrequency(1500);
     */
    JMotorDriverPCA9685HBridge(PCA9685& _pca9685, byte _channelPos, byte _channelNeg, bool _reverse = false, bool _breakWhenEnabled = true, bool _breakWhenDisabled = false)
        : pca9685(_pca9685)
    {
        channelPos = _channelPos;
        channelNeg = _channelNeg;
        enabled = false;
        reverse = _reverse;
        breakWhenEnabled = _breakWhenEnabled;
        breakWhenDisabled = _breakWhenDisabled;
    }
    /**
     * @brief  activate electrical break mode when motor is enabled and speed is 0
     */
    void setBreakWhenEnabled(bool _breakWhenEnabled)
    {
        breakWhenEnabled = _breakWhenEnabled;
    }
    /**
     * @brief  activate electrical break mode when motor is disabled
     */
    void setBreakWhenDisabled(bool _breakWhenDisabled)
    {
        breakWhenDisabled = _breakWhenDisabled;
    }
    void setReverse(bool _reverse)
    {
        reverse = _reverse;
    }
    bool set(float val)
    {
        if (reverse) {
            val = -val;
        }
        const byte dutyCycleMax = pca9685.getDutyCycleMax();
        if (enabled) {
            val = val * dutyCycleMax;
            val = constrain(val, -dutyCycleMax, dutyCycleMax);
            if (val == 0) {
                if (breakWhenEnabled) {
                    pca9685.setChannelDutyCycle(channelPos, dutyCycleMax);
                    pca9685.setChannelDutyCycle(channelNeg, dutyCycleMax);
                } else {
                    pca9685.setChannelDutyCycle(channelPos, 0);
                    pca9685.setChannelDutyCycle(channelNeg, 0);
                }
            }
            if (val > 0) {
                pca9685.setChannelDutyCycle(channelNeg, 0);
                pca9685.setChannelDutyCycle(channelPos, val);
            } else {
                pca9685.setChannelDutyCycle(channelPos, 0);
                pca9685.setChannelDutyCycle(channelNeg, -val);
            }
        }
        return abs(val) < dutyCycleMax;
    }
    bool setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                // actually enable
                if (breakWhenEnabled) {
                    pca9685.setChannelDutyCycle(channelPos, pca9685.getDutyCycleMax());
                    pca9685.setChannelDutyCycle(channelNeg, pca9685.getDutyCycleMax());
                } else {
                    pca9685.setChannelDutyCycle(channelPos, 0);
                    pca9685.setChannelDutyCycle(channelNeg, 0);
                }
                enabled = true;
                return true;
            }
        } else { // disable
            if (enabled) {
                // actually disable
                if (breakWhenDisabled) {
                    pca9685.setChannelDutyCycle(channelPos, pca9685.getDutyCycleMax());
                    pca9685.setChannelDutyCycle(channelNeg, pca9685.getDutyCycleMax());
                } else {
                    pca9685.setChannelDutyCycle(channelPos, 0);
                    pca9685.setChannelDutyCycle(channelNeg, 0);
                }
                enabled = false;
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

#endif // JMOTOR_DRIVER_PCA9685HBRIDGE_H
