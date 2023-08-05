
#include "main.hpp"

// Deep sleep parameters
u_int32_t lastActivity = 0;
u_int32_t sleepTimeout = 10000;
bool justWokeUp = true;

Button triggerBtn = {GPIO_NUM_32, 0, false};

/* Tasks */

/* Prototypes */
void StartEffects();

void print_wakeup_reason();
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
  justWokeUp = false;
  if (triggerBtn.pressed)
  {
    StartEffects();
    lastActivity = millis();
  }
  else
  {
    StopAudio();
  }
}

void SleepTask(void *parameter)
{
  while (true)
  {
    if ((millis() - lastActivity) > sleepTimeout)
    {
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1); // 1 = High, 0 = Low
      Serial.println("Going to sleep");
      ClearLeds();
      esp_deep_sleep_start();
      vTaskDelete(nullptr);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void setup()
{

  Serial.begin(115200); // Not needed for sound, just to demo printing to the serial
  delay(1000);

  justWokeUp = true;
  triggerBtn.pressed = true;
  attachInterrupt(triggerBtn.PIN, ButtonTask, CHANGE);

  SetUpLeds();

  print_wakeup_reason();

  xTaskCreate(
      SleepTask,
      "Wait for sleep",
      4096,
      nullptr,
      1,
      nullptr);
}

void loop()
{
  vTaskDelay(100000 / portTICK_PERIOD_MS);
}