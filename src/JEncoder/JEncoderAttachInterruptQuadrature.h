/**
 * @brief  reads a quadrature (incremental) encoder using attachInterrupt()
 * @note  platform: esp32, teensy
 *          (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */
#ifndef J_ENCODER_ATTACH_INTERRUPT_QUADRATURE_H
#define J_ENCODER_ATTACH_INTERRUPT_QUADRATURE_H
#include "JEncoder.h"
class JEncoderAttachInterruptQuadrature : private JEncoder {
};
#endif
