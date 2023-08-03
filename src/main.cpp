
#include "main.hpp"

// Deep sleep parameters
u_int32_t lastActivity = 0;
u_int32_t sleepTimeout = 10000;
bool justWokeUp = true;

// Define the array of leds
CRGB leds[NUM_LEDS];

/* AUDIO */
XT_Wav_Class LaserSound(laser_wav);   // create an object of type XT_Wav_Class that is used by
                                      // the dac audio class (below), passing wav data as parameter.
XT_Wav_Class GlitchSound(glitch_wav); // create an object of type XT_Wav_Class that is used by
                                      // the dac audio class (below), passing wav data as parameter.
XT_DAC_Audio_Class DacAudio(25, 0);   // Create the main player class object.
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0

Button triggerBtn = {GPIO_NUM_32, 0, false};

/* Tasks */
TaskHandle_t audioTaskHandle = nullptr;
TaskHandle_t ledTaskHandle = nullptr;
u_int32_t taskCount = 0;

/* Prototypes */
void StartEffects();
void PlayAudio(void *parameter);
void PlayLedBoot();
void PlayLedShoot();
void PlayLeds(void *parameter);
void print_wakeup_reason();
/* Functions */

void StartEffects()
{

  // xTaskCreatePinnedToCore(
  //     PlayAudio,
  //     "Play audio",
  //     10000,
  //     nullptr,
  //     1,
  //     &audioTaskHandle,
  //     1);
  if (ledTaskHandle == nullptr)
  {
    xTaskCreatePinnedToCore(
        PlayLeds,
        "Play leds",
        4096,
        nullptr,
        2,
        &ledTaskHandle,
        0);
  }
}

void PlayAudio(void *parameter)
{

  Serial.println("Starting Sound");
  taskCount++;
  Serial.println(taskCount);
  DacAudio.FillBuffer();
  if (LaserSound.Playing == false) // OLaserSound.Playing ) //
  {
    Serial.println("Playing Sound");
    DacAudio.Play(&LaserSound); //                play it, this will cause it to repeat and repeat...
    DacAudio.Play(&GlitchSound, true);
    vTaskDelay(10); /// portTICK_PERIOD_MS);
  }

  Serial.println("Ending Sound");
  DacAudio.StopAllSounds();

  // When you're done, call vTaskDelete. Don't forget this!
  vTaskDelete(nullptr);
}

void PlayLedBoot()
{
}

void PlayLedShoot()
{
}

void PlayLeds(void *parameter)
{
  PlayLedBoot();
  PlayLedShoot();
  // play it, this will cause it to repeat and repeat...
  // Turn the LED on, then pause
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Red;
    FastLED.show();
    vTaskDelay((100 / portTICK_PERIOD_MS) / (i * i + 1));
    // Now turn the LED off, then pause
    leds[i] = CRGB::Black;
    FastLED.show();
    vTaskDelay((100 / portTICK_PERIOD_MS) / (i * i + 1));
  }
  // When you're done, call vTaskDelete. Don't forget this!
  vTaskDelete(nullptr);
}

void IRAM_ATTR ButtonTask()
{

  triggerBtn.numberKeyPresses++;
  triggerBtn.pressed = !triggerBtn.pressed || justWokeUp; // Switch so it would disable tasks on let go
  justWokeUp = false;
  if (triggerBtn.pressed)
  {
    StartEffects();
    lastActivity = millis();
  }
  else
  {
    // Stop tasks
    // vTaskDelete(audioTaskHandle);
    if (ledTaskHandle != nullptr)
    {
      eTaskState ledState = eTaskGetState(ledTaskHandle);
      if(ledState!=eDeleted)
        {
          vTaskDelete(ledTaskHandle);
          ledTaskHandle = nullptr;
        }
    }
    // Serial.println("Ending Sound");
    //  DacAudio.StopAllSounds();
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

  attachInterrupt(triggerBtn.PIN, ButtonTask, CHANGE);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical

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