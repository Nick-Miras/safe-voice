//
// Created by Nick Anthony Miras on 3/17/26.
//
#include "audio.h"

// ---------------------------------------------------
// Pico 2 Wiring to INMP441
// ---------------------------------------------------
// L/R to GND (Sets mic to Left channel)
// VDD to 3.3V
// GND to GND
#define I2S_BCLK 26  // Connect to SCK on INMP441
                     // NOTE: WS is automatically BCLK + 1 (Pin 27)
#define I2S_DIN  28  // Connect to SD on INMP441

void setupMicrophone(I2S i2sIn) {
    // Assign the GPIO pins to the I2S interface
    i2sIn.setBCLK(I2S_BCLK);
    i2sIn.setDATA(I2S_DIN);

    // The INMP441 generates 24-bit data, but standard I2S
    // protocols pad it into a 32-bit frame.
    i2sIn.setBitsPerSample(32);
}