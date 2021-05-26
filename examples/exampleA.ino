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

JVoltageCompMeasure<10> voltageComp = JVoltageCompMeasure<10>(batMonitorPin, dacUnitsPerVolt);
JMotorCompStandardConfig ttConfig = JMotorCompStandardConfig(1.9, .5, 3.2, 1.1, 4.6, 1.7, 100);
JMotorCompStandard myMotorCompensator = JMotorCompStandard(voltageComp, ttConfig, 1.0);
JMotorDriverEsp32L293 myDriver = JMotorDriverEsp32L293(portD);
JControlLoopBasic myCtrlLoop = JControlLoopBasic(10);
JMotorControllerClosed myController = JMotorControllerClosed(myDriver, myMotorCompensator, encoder, myCtrlLoop, 1.5, .4, true, 1.5, .25);
// JMotorControllerOpen myController = JMotorControllerOpen(myDriver,myMotorCompensator);
String inString = "";
float value = 0;
int mode = 0;

void setup()
{
    Serial.begin(250000);
    encoder.setUpInterrupts(encoder_jENCODER_ISR);
    myController.enable();
    // myDriver.enable();
}
void loop()
{
    while (Serial.available() > 0) {
        int inChar = Serial.read();
        inString += (char)inChar;
        if (inChar == '\n') {
            if (inString.equals("e\n")) {
                myController.setEnable(!myController.getEnable());
            } else if (inString.equals("r\n")) {
                myController.resetPos();
            } else if (inString.equals("p\n")) {
                mode = 1;
            } else if (inString.equals("v\n")) {
                mode = 0;
            } else {
                value = inString.toFloat();
                if (mode == 1)
                    myController.setPosSetpoint(value, false);
                if (mode == 0)
                    myController.setOpenVel(value, false);
            }
            inString = "";
        }
    }
    myController.run();

    Serial.print(myController.getVel());
    Serial.print(",");
    Serial.print(myController.getVelTarget());
    Serial.print(",");
    Serial.print(myController.getPosDeltaSetpoint());
    Serial.print(",");
    Serial.print(myController.getVelSetpoint());
    Serial.print(",");
    Serial.print(myController.getPosTarget());
    Serial.print(",");
    Serial.print(myController.getPosSetpoint());
    Serial.print(",");
    Serial.print(myController.getPos());
    Serial.print(",");
    Serial.print(myController.getVelSetpoint());
    Serial.println();

    // delay(2);
}
