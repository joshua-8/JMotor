#include "JMotor.h"
#include <Arduino.h>
JMotorDriverEsp32L293 jmd(1, 5, 18, 21);
void setup()
{
}

void loop()
{
    jmd.setEnable(true);
    delay(2000);
    for (float i = -1.2; i < 1.2; i += .0025) {
        jmd.set(i);
        delay(30);
    }
    jmd.setEnable(false);
    delay(2000);
}