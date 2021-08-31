#include <Arduino.h>

//gentle servo example

#include <JMotor.h>

#define port3Pin 25
#define port3 6, port3Pin
#define inport1 39

JMotorDriverEsp32Servo servoDriver = JMotorDriverEsp32Servo(port3);
JServoCurrentSensor<20> stallSensor = JServoCurrentSensor<20>(inport1);
JServoControllerAdvanced servoCtrl = JServoControllerAdvanced(servoDriver);
JServoControllerGentle servoSP = JServoControllerGentle(servoCtrl, stallSensor);

String inString = "";
float value = 0;
int mode = 0;

void setup()
{
    Serial.begin(115200);
    servoSP.setAccelLimit(50);
    servoSP.enable();
}
void loop()
{
    if (millis() % 40000 > 10000) {
        servoSP.setAngleSmoothed(120);
    } else {
        servoSP.gripGently(false);
    }
}
