#include "JMotor.h"
#include <Arduino.h>
JMotorDriverAvrPWM m1 = JMotorDriverAvrPWM(9);
JMotorDriverAvrPWM m2 = JMotorDriverAvrPWM(10);
JMotorDriverDual driver2 = JMotorDriverDual(m1, m2);
JMotorDriverAvrPWM m3 = JMotorDriverAvrPWM(11);
JMotorDriverDual tripleDriver = JMotorDriverDual(m1, m2);
void setup()
{
    tripleDriver.enable();
}

void loop()
{
    tripleDriver.set(.25);
    delay(1000);
    tripleDriver.set(.75);
    delay(2000);
}
