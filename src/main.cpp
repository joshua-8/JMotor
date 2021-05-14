#include <Arduino.h>
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

//
#define dacUnitsPerVolt 380

JMotorDriverEsp32L293 myDriver = JMotorDriverEsp32L293(portD);

JEncoderPWMAbsoluteAttachInterrupt encoder = JEncoderPWMAbsoluteAttachInterrupt(port1Pin, JEncoderPWMAbsolute_AS5048settings, true, 1, 50000, 1000, true);
IRAM_ATTR jENCODER_MAKE_ISR_MACRO(encoder);

JVoltageCompMeasure<10> voltageComp = JVoltageCompMeasure<10>(batMonitorPin, dacUnitsPerVolt);
JMotorCompStandardConfig ttConfig = JMotorCompStandardConfig(1.9, .5, 3.2, 1.1, 4.6, 1.7, 100);
JMotorCompStandard myMotorCompensator = JMotorCompStandard(voltageComp, ttConfig, 1.0);
JMotorControllerOpen myController = JMotorControllerOpen(myDriver, myMotorCompensator, INFINITY, .5);

String inString = "";
float value = 0;
void setup()
{
    Serial.begin(9600);
    encoder.setUpInterrupts(encoder_jENCODER_ISR);
    myController.enable();
}
void loop()
{

    while (Serial.available() > 0) {
        int inChar = Serial.read();
        inString += (char)inChar;
        if (inChar == '\n') {
            if (inString.equals(" \n")) {
                myController.setEnable(!myController.getEnabled());
                inString = "";
            } else if (inString.equals("r\n")) {
                myController.resetPos();
                inString = "";
            } else {
                value = inString.toFloat();
                inString = "";

                myController.setPosTarget(value,true);
            }
        }
    }
    encoder.run();
    myController.run();
    Serial.print(myController.getPosTarget());
    Serial.print(",");
    Serial.println(myController.getPos());
    delay(10);
}
