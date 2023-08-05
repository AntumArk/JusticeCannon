#pragma once
#include <Arduino.h>
#include "laser.h"
#include "glitch.h"
#include "XT_DAC_Audio.h"

#define DAC_GPIO_PIN 25
#define DAC_TIMER 0
#define AUDIO_CORE 1
#define AUDIO_PRIORITY 1
#define AUDIO_STACK_SIZE 10000
//#define AUDIO_DEBUG

void PlayAudio(void *parameter);
void StopAudio();
void StartAudioTask();