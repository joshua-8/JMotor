#ifndef J_MOTOR_DRIVER_AVR_L293D_H
#define J_MOTOR_DRIVER_AVR_L293D_H
#include "JMotorDriver.h"
#include "JMotorDriverAvrPWM.h"
class JMotorDriverAvrL293d : public JMotorDriver {
private:
    boolean enabled = false;
    int i1;
    int i2;

public:
    JMotorDriverAvrPWM pwmDriver;
    JMotorDriverAvrL293d(int _en, int _i1, int _i2)
        : pwmDriver { _en }
    {
        enabled = false;
        i1 = _i1;
        i2 = _i2;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::avrL293d;
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
                digitalWrite(i2, LOW);
                digitalWrite(i1, HIGH);
            } else if (val < 0) {
                digitalWrite(i1, LOW);
                digitalWrite(i2, HIGH);
            } else {
                digitalWrite(i1, HIGH);
                digitalWrite(i2, HIGH);
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
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, HIGH);
                digitalWrite(i2, HIGH);
                pwmDriver.setEnable(true);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                pwmDriver.setEnable(false);
                pinMode(i1, OUTPUT);
                pinMode(i2, OUTPUT);
                digitalWrite(i1, LOW);
                digitalWrite(i2, LOW);
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