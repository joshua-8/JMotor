#ifndef J_MOTOR_DRIVER_AVR_PWMDIR_H
#define J_MOTOR_DRIVER_AVR_PWMDIR_H
#include "JMotorDriver.h"
#include "JMotorDriverAvrPWM.h"
class JMotorDriverAvrPWMDir : public JMotorDriver {
private:
    boolean enabled = false;
    int dir;

public:
    boolean reverse = false;
    JMotorDriverAvrPWM pwmDriver;
    /**
     * @brief  
     * @note   
     * @param  _speedPin: 
     * @param  _dirPin: 
     * @param  _rev: 
     * @retval 
     */
    JMotorDriverAvrPWMDir(int _speedPin, int _dirPin, boolean _rev = false)
        : pwmDriver { _speedPin }
    {
        enabled = false;
        dir = _dirPin;
        reverse = _rev;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::avrPWMDir;
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