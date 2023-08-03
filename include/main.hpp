#pragma once
#include <Arduino.h>
#include "laser.h"
#include "glitch.h"
#include "XT_DAC_Audio.h"
#include <FastLED.h>


struct Button
{
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

// How many leds in your strip?
#define NUM_LEDS 13

// LED Data pin
#define DATA_PIN 4