#pragma once
#include <Arduino.h>
#include <FastLED.h>
// How many leds in your strip?
#define NUM_LEDS 13

// LED Data pin
#define DATA_PIN 4
#define LED_CPU_CORE 0
#define LED_CPU_PRIORITY 2
#define LED_STACK_SIZE 4096

void PlayLedBoot();
void PlayLedShoot();
void PlayLeds(void *parameter);
void CreateLedTask();
void ClearLeds();
void SetUpLeds();