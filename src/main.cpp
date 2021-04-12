#include <Arduino.h>
#include "JMotor.h"
JMotorDriverAvrPWMDir jmd(9,8);
void setup()
{
}

void loop()
{
    jmd.setEnable(true);
    delay(2000);
    for (float i = -1.2; i < 1.2; i += .01) {
        jmd.set(i);
        delay(30);
    }
    jmd.setEnable(false);
    delay(2000);
}