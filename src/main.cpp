#include <Arduino.h>
#include "laser.h"
#include "glitch.h"
#include "XT_DAC_Audio.h"
#include <FastLED.h>

u_int32_t lastActivity = 0;
u_int32_t sleepTimeout = 10000;
// How many leds in your strip?
#define NUM_LEDS 13

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 4

// Define the array of leds
CRGB leds[NUM_LEDS];

XT_Wav_Class LaserSound(laser_wav);   // create an object of type XT_Wav_Class that is used by
                                      // the dac audio class (below), passing wav data as parameter.
XT_Wav_Class GlitchSound(glitch_wav); // create an object of type XT_Wav_Class that is used by
                                      // the dac audio class (below), passing wav data as parameter.
XT_DAC_Audio_Class DacAudio(25, 0);   // Create the main player class object.
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0

uint32_t DemoCounter = 0; // Just a counter to use in the serial monitor
                          // not essential to playing the sound
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
  pinMode(GPIO_NUM_32, INPUT);
  // Monitor whilst the sound plays, ensure your serial monitor
  // speed is set to this speed also.
  // Uncomment/edit one of the following lines for your leds arrangement.
  // ## Clockless types ##
  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // GRB ordering is assumed
  // FastLED.addLeds<SM16703, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1829, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1812, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1904, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS2903, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
  // FastLED.addLeds<WS2852, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
   FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
  // FastLED.addLeds<GS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
  // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<APA106, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<PL9823, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2813, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2811_400, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GE8822, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD1886, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD1886_8BIT, DATA_PIN, RGB>(leds, NUM_LEDS);
  // ## Clocked (SPI) types ##
  // FastLED.addLeds<LPD6803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
  // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
  // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
  // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
  // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
  // FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
  print_wakeup_reason();
}

void loop()
{

  if ((millis() - lastActivity) > sleepTimeout)
  {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 1); // 1 = High, 0 = Low
    Serial.println("Going to sleep");
    esp_deep_sleep_start();
  }

  if (digitalRead(GPIO_NUM_32))
  {
      // initialize the x/y and time values
  // random16_set_seed(8934);
  // random16_add_entropy(analogRead(3));


    lastActivity = millis();
    DacAudio.FillBuffer();           // Fill the sound buffer with data
    if (LaserSound.Playing == false) // if not playing,
    {
      DacAudio.Play(&LaserSound); //                play it, this will cause it to repeat and repeat...
      DacAudio.Play(&GlitchSound, true);
    } //                play it, this will cause it to repeat and repeat...
    // Turn the LED on, then pause
    for (size_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB::Cyan;
      FastLED.show();
      delay(50);
      // Now turn the LED off, then pause
      leds[i] = CRGB::Black;
      FastLED.show();
      delay(50);
    }
  }
  else
  {
    DacAudio.StopAllSounds();
  }
}