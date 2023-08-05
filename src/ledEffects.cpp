#include "ledEffects.hpp"

// Define the array of leds
CRGB leds[NUM_LEDS];
TaskHandle_t ledTaskHandle = nullptr;
volatile u_int32_t taskCounter = 0;

void CreateLedTask()
{
    xTaskCreatePinnedToCore(
        PlayLeds,
        "Play leds",
        LED_STACK_SIZE,
        nullptr,
        LED_CPU_PRIORITY,
        &ledTaskHandle,
        LED_CPU_CORE);
}

void ClearLeds()
{
    for (size_t i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB::Black;
        FastLED.show();
    }
}

void SetUpLeds()
{
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); // GRB ordering is typical
    leds[0] = CRGB::PaleVioletRed;                           // indicate power On
    FastLED.show();
}

void PlayLedBoot()
{
    FastLED.setBrightness(255);
    pinMode(GPIO_NUM_33, INPUT);
    randomSeed(analogRead(GPIO_NUM_33));
    uint8_t cyclesToTrigger = random(5, 10);
    uint8_t cycleDelay = random(30, 100);
    for (size_t i = 0; i < cyclesToTrigger; i++)
    {
        ClearLeds();
        for (size_t i = 0; i < NUM_LEDS; i++)
        {
            uint8_t brigntessDivisor = random(1, 5);
            uint8_t colorVarience = random(0, 50);
            leds[i] = CRGB(35 / brigntessDivisor + colorVarience, 152 / brigntessDivisor + colorVarience, 176 / brigntessDivisor + colorVarience) * random(0, 2);
            FastLED.show();
        }
        vTaskDelay(cycleDelay / portTICK_PERIOD_MS);
    }
    ClearLeds();
}

void PlayLedShoot()
{
    uint16_t shootTime = 6000;
    for (size_t j = 0; j < 255; j++)
    {
        for (size_t i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB::LimeGreen;
        }
        FastLED.setBrightness(j);
        FastLED.show();
        vTaskDelay((shootTime / 255) / portTICK_PERIOD_MS);
    }

    uint16_t powerDownTime = 200;
    for (size_t j = 255; j > 0; j--)
    {
        FastLED.setBrightness(j);
        FastLED.show();
        vTaskDelay((powerDownTime / 255) / portTICK_PERIOD_MS);
    }
}

void PlayLeds(void *parameter)
{
    Serial.println("Led");
    Serial.println(taskCounter);
    taskCounter++;
    if (taskCounter == 1)
    {
        PlayLedBoot();
        PlayLedShoot();
    }
    vTaskDelay(10);
    taskCounter--;
    // When you're done, call vTaskDelete. Don't forget this!
    vTaskDelete(nullptr);
}
