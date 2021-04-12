#ifndef J_MOTOR_DRIVER_ESP32_PWMDIR_H
#define J_MOTOR_DRIVER_ESP32_PWMDIR_H
#include "JMotorDriver.h"
#include "JMotorDriverEsp32PWM.h"
class JMotorDriverESP32L293d : private JMotorDriver {
private:
    boolean enabled = false;
    int dir;

public:
    JMotorDriverEsp32PWM pwmDriver;
    boolean reverse = false;

    JMotorDriverESP32L293d(int _ch, int _enablePin, int _dirPin, boolean _rev = false)
        : pwmDriver { _ch, _enablePin }
    {
        enabled = false;
        dir = _dirPin;
    }
    JMotorDriverESP32L293d(int _ch, int _enPin, int _dirPin, int freq, int resolution, boolean _rev = false)
        : pwmDriver { _ch, _enPin, freq, resolution }
    {
        enabled = false;
        dir = _dirPin;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::esp32PWMDir;
    }
    /**
     * @brief  
     * @note   
     * @param  _val: 
     * @retval true if value at end of range
     */
    boolean set(float val)
    {
        if (enabled) {
            if (val > 0) {
                digitalWrite(dir, !reverse);
            } else if (val < 0) {
                digitalWrite(dir, reverse);
            } else {
            }
            pwmDriver.set(abs(val));
        }
        return abs(val) >= 1.0;
    }
    /**
     * @brief  
     * @note   
     * @param  enable: 
     * @retval true if state changed
     */
    boolean setEnable(boolean enable)
    {
        if (enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                pinMode(dir, OUTPUT);
                pwmDriver.setEnable(true);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
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