#include <Arduino.h>

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

// JMotorDriverEsp32Servo myServo = JMotorDriverEsp32Servo(port1);
// JServoCurrentSensor myServoCurrent = JServoCurrentSensor(port5Pin, 150);
// JServoControllerStallProtected servoCtrl = JServoControllerStallProtected(JServoControllerAdvanced(myServo, .3, 0, 1, 0, false, 120, 75, 0, -90, 90, 0, -90, 90), myServoCurrent, .1, .4, 500, 1000);
// JServoControllerAdvanced servoCtrl = JServoControllerAdvanced(myServo, .3, 0, 1, 1000, false, 120, 75, 0, -40, 40, 0, -90, 90, 544, 2400, true, true, 1.0);

JEncoderPWMAbsoluteAttachInterrupt encoder = JEncoderPWMAbsoluteAttachInterrupt(inport2, JEncoderPWMAbsolute_AS5048settings, true, 1, 50000, 1000, true);
IRAM_ATTR jENCODER_MAKE_ISR_MACRO(encoder);

JVoltageCompMeasure<25> voltageComp = JVoltageCompMeasure<25>(batMonitorPin, dacUnitsPerVolt);
JMotorCompStandardConfig ttConfig = JMotorCompStandardConfig(1.9, .5, 3.2, 1.1, 4.6, 1.7, 100);
JMotorCompStandard myMotorCompensator = JMotorCompStandard(voltageComp, ttConfig, 1.0);
JMotorDriverEsp32L293 myDriver = JMotorDriverEsp32L293(portD);
JControlLoopBasic myCtrlLoop = JControlLoopBasic(10, 1000);
JMotorControllerClosed myController = JMotorControllerClosed(myDriver, myMotorCompensator, encoder, myCtrlLoop, 1.5, .4, true, 1.5, .25);
JDrivetrainTwoSide drivetrain = JDrivetrainTwoSide(myController, myController, 1);
JDrivetrainControllerBasic mydrvtrain = JDrivetrainControllerBasic(drivetrain, { 1, 1, 0 }, { 2, 2, 1 }, { .01, .01, .01 });

String inString = "";
float value = 0;
int mode = 0;

void setup()
{
    Serial.begin(250000);
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

            } else if (inString.equals("r\n")) {

            } else if (inString.equals("p\n")) {

            } else if (inString.equals("v\n")) {

            } else {
                value = inString.toFloat();
            }
            inString = "";
        }
    }
    mydrvtrain.run();
}