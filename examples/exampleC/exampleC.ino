#include <Arduino.h>

//servo example

#include <JMotor.h>

#define port1Pin 32
#define port2Pin 33
#define port3Pin 25
#define port4Pin 26
#define port5Pin 27
//      PWM_CH, PIN
#define port1 8, port1Pin
#define port2 7, port2Pin
#define port3 6, port3Pin
#define port4 5, port4Pin
#define port5 4, port5Pin
//      PWM_CH, EN_PIN, PIN1, PIN2
#define portA 3, 23, 22, 19
#define portB 1, 5, 21, 18
#define portC 2, 17, 16, 12
#define portD 0, 13, 14, 4

#define inport1 39
#define inport2 34
#define inport3 35

#define batMonitorPin 36

#define dacUnitsPerVolt 380

JMotorDriverEsp32Servo servoDriver = JMotorDriverEsp32Servo(port3);
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
                if (mode == 0) { //q
                    servo.setAngleSmoothed(value);
                }
                if (mode == 1) { //w
                    servo.setAngleImmediate(value);
                }
                if (mode == 2) { //e
                    servo.setVelLimit(value);
                }
                if (mode == 3) { //r
                    servo.setAccelLimit(value);
                }
            }
            inString = "";
        }
    }
    servo.run();
}
