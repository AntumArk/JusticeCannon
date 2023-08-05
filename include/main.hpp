#pragma once
#include <Arduino.h>
#include "ledEffects.hpp"
#include "gunAudio.hpp"

struct Button
{
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};
