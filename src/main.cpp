#include <Arduino.h>
#include <JMotor.h>
JMotorDriverAvrL293 myDriver = JMotorDriverAvrL293(9, 10, 11);
float voltage = 5.0;
JVoltageCompAdjust voltageComp = JVoltageCompAdjust(voltage);
JMotorCompStandard myMotorCompensator = JMotorCompStandard(voltageComp, JMotorCompStandardConfig(0.1, 3, 1.5, .5, .3, 50));
JMotorControllerBasicOpen myController = JMotorControllerBasicOpen(myDriver, myMotorCompensator);

float vel = 0;
boolean inc = true;
void setup()
{
    Serial.begin(9600);
    myController.enable();
}

void loop()
{
    inc ? vel += .001 : vel -= .001;
    if (abs(vel) > 1)
        inc = !inc;

    myController.setVel(vel);
    delay(10);
}
