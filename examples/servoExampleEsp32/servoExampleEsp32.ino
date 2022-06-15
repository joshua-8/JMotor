#include <Arduino.h>

// servo example

#include <JMotor.h>

JMotorDriverEsp32Servo servoDriver = JMotorDriverEsp32Servo(6, 25);
JServoControllerAdvanced servo = JServoControllerAdvanced(servoDriver);

String inString = "";
float value = 0;
int mode = 0;

void setup()
{
    Serial.begin(115200);
}
void loop()
{
    while (Serial.available() > 0) {
        int inChar = Serial.read();
        inString += (char)inChar;
        if (inChar == '\n') {
            if (inString.equals(" \n")) {
                servo.setEnable(!servo.getEnable());
            } else if (inString.equals("q\n")) {
                mode = 0;
            } else if (inString.equals("w\n")) {
                mode = 1;
            } else if (inString.equals("e\n")) {
                mode = 2;
            } else if (inString.equals("r\n")) {
                mode = 3;
            } else {
                value = inString.toFloat();
                // space=enable/disable
                if (mode == 0) { // q
                    servo.setAngleSmoothed(value);
                }
                if (mode == 1) { // w
                    servo.setAngleImmediate(value);
                }
                if (mode == 2) { // e
                    servo.setVelLimit(value);
                }
                if (mode == 3) { // r
                    servo.setAccelLimit(value);
                }
            }
            inString = "";
        }
    }
    servo.run();
}
