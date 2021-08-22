#include <Arduino.h>

//motor and encoder example (for AVR)

#include <JMotor.h>

JEncoderAS5048bI2C encoder = JEncoderAS5048bI2C(true, 3.0/37.5);
JMotorDriverAvrPWMDir driver = JMotorDriverAvrPWMDir(10, 9);
JMotorCompDirect comp = JMotorCompDirect(1);
JControlLoopBasic ctrlLoop = JControlLoopBasic(20, 1000);
JMotorControllerClosed ctrl = JMotorControllerClosed(driver, comp, encoder, ctrlLoop, 1.5, 1, .5, false, 3.0);

String inString = "";
float value = 0;
int mode = 0;

void setup()
{
    encoder.setVelEnoughTicks(50000);
    encoder.setVelEnoughTime(1000);
    encoder.setRecognizeOutOfRange(false);
    Wire.begin();
    Serial.begin(2000000);
    ctrl.setEnable(true);
}
void loop()
{
    while (Serial.available() > 0) {
        int inChar = Serial.read();
        inString += (char)inChar;
        if (inChar == '\n') {
            if (inString.equals(" \n")) {
                ctrl.setEnable(!ctrl.getEnable());
            } else if (inString.equals(";\n")) {
                ctrl.resetPos();
            } else if (inString.equals("q\n")) {
                mode = 0;
            } else if (inString.equals("w\n")) {
                mode = 1;
            } else if (inString.equals("e\n")) {
                mode = 2;
            } else if (inString.equals("r\n")) {
                mode = 3;
            } else if (inString.equals("t\n")) {
                mode = 4;
            } else {
                value = inString.toFloat();
                if (mode == 0) { //q
                    ctrl.setVel(value);
                }
                if (mode == 1) { //w
                    ctrl.setPosTargetStallable(value);
                }
                if (mode == 2) { //e
                    ctrlLoop.setKP(value);
                }
                if (mode == 3) { //r
                    ctrl.setOpenVel(value);
                }
                if (mode == 4) { //t
                    comp.setMultiplier(value);
                }
            }
            inString = "";
        }
    }
    Serial.print("vel:");
    Serial.print(ctrl.getVel());
    Serial.print(" pos:");
    Serial.println(ctrl.getPos());
    ctrl.run();
}
