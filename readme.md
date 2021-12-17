# JMotor Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/JMotor.svg?)](https://www.ardu-badge.com/JMotor)
Also available for [PlatformIO](https://platformio.org/lib/show/12230/JMotor)

## work in progress! Bug reports/fixes welcomed

https://github.com/joshua-8/JMotor

reference docs (made with Doxygen): https://joshua-8.github.io/JMotor/md_readme.html

list of all classes: https://joshua-8.github.io/JMotor/hierarchy.html

This library defines common interfaces for reading encoders and controlling motors allowing for easy open or closed loop motor control.

Supports reading the following kinds of encoders: quadrature, single pin, absolute with PWM output, AS5048b with I2C.

Supports controlling the following kinds of motor drivers: single PWM pin, PWM and direction pins, servo signal, L293/L298 driver. 

supports the following boards:   ESP32, Arduino AVR (tested on nano), Teensy (tested on LC), SAMD M0 boards (tested on feather m0)

Why do all the classes start with J? It's to avoid name conflicts with other libraries (and J is my first initial).

This library uses lots of floating point math, and in general isn't optimized for speed or small size. Ease of use and ease of development is prioritized.
