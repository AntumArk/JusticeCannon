
#include "main.hpp"

Button triggerBtn = {GPIO_NUM_32, 0, false};

/* Tasks */

/* Prototypes */
void StartEffects();

/* Functions */

void StartEffects()
{
  StartAudioTask();
  CreateLedTask();
}

void IRAM_ATTR ButtonTask()
{

  triggerBtn.numberKeyPresses++;
  triggerBtn.pressed = digitalRead(triggerBtn.PIN); // Switch so it would disable tasks on let go
  if (triggerBtn.pressed)
  {
    StartEffects();
    UpdateActivity();
  }
  else
  {
    StopAudio();
  }
}

void setup()
{
  Serial.begin(115200); // Not needed for sound, just to demo printing to the serial
  delay(1000);

  triggerBtn.pressed = true;
  attachInterrupt(triggerBtn.PIN, ButtonTask, CHANGE);

  SetUpLeds();

  print_wakeup_reason();
}

void loop()
{
  vTaskDelay(100000 / portTICK_PERIOD_MS);
}