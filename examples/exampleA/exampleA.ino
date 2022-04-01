#include <Arduino.h>

//drivetrain example

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

#include <JMotor.h>

JEncoderPWMAbsoluteAttachInterrupt encoder = JEncoderPWMAbsoluteAttachInterrupt(inport2, JEncoderPWMAbsolute_AS5048settings, true, 1, 50000, 1000, true);
IRAM_ATTR jENCODER_MAKE_ISR_MACRO(encoder);

JVoltageCompMeasure<25> voltageComp = JVoltageCompMeasure<25>(batMonitorPin, dacUnitsPerVolt);
JMotorCompStandardConfig ttConfig = JMotorCompStandardConfig(1.9, .5, 3.2, 1.1, 4.6, 1.7, 100);
JMotorCompStandard myMotorCompensator = JMotorCompStandard(voltageComp, ttConfig, 1.0);
JMotorDriverEsp32L293 myDriver = JMotorDriverEsp32L293(portD);
JControlLoopBasic myCtrlLoop = JControlLoopBasic(10, 1000);
JMotorControllerClosed myController = JMotorControllerClosed(myDriver, myMotorCompensator, encoder, myCtrlLoop, INFINITY, INFINITY, 0.25, true, 1.5);

JMotorDriverEsp32L293 otherDriver = JMotorDriverEsp32L293(portA);
JMotorCompStandard otherCompensator = JMotorCompStandard(voltageComp, ttConfig, 1.0);
JMotorControllerOpen otherController = JMotorControllerOpen(otherDriver, otherCompensator);

JDrivetrainTwoSide drivetrain = JDrivetrainTwoSide(myController, otherController, 360 / HALF_PI);
JDrivetrainControllerBasic mydrvtrain = JDrivetrainControllerBasic(drivetrain, { 1, 1, 0 }, { .2, .2, 0 }, { .5, .5, 0 });

String inString = "";
float value = 0;
int mode = 0;

void setup()
{
    Serial.begin(115200);
    encoder.setUpInterrupts(encoder_jENCODER_ISR);
    mydrvtrain.enable();
}
void loop()
{
    while (Serial.available() > 0) {
        int inChar = Serial.read();
        inString += (char)inChar;
        if (inChar == '\n') {
            if (inString.equals("e\n")) {
                mydrvtrain.setEnable(!mydrvtrain.getEnable());
            } else if (inString.equals("s\n")) {
                mode = 1;
            } else if (inString.equals("x\n")) {
                mode = 0;
            } else if (inString.equals("v\n")) {
                mode = 2;
            } else if (inString.equals("p\n")) {
                mode = 3;
            } else if (inString.equals(";\n")) {
                mode = 4;
            } else if (inString.equals("r\n")) {
                mydrvtrain.resetDist();
            } else {
                value = inString.toFloat();
                if (mode == 1) { //s
                    mydrvtrain.setVel({ value, 0, 0 });
                }
                if (mode == 0) { //x
                    mydrvtrain.setVel({ 0, value, 0 });
                }
                if (mode == 2) { //v
                    mydrvtrain.moveVel({ value, 0, 0 });
                }
                if (mode == 3) { //p
                    mydrvtrain.moveDistRZ(value);
                }
                if (mode == 4) { //;
                    mydrvtrain.moveDistY(value);
                }
            }
            inString = "";
        }
    }
    mydrvtrain.run();
    delay(5);
}
