#pragma once
#include <Arduino.h>
// Deep sleep parameters
constexpr u_int32_t sleepTimeout = 10000;

void CreateSleepTask();
void print_wakeup_reason();
void UpdateActivity();
