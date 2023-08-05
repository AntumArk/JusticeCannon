/* AUDIO */
#include "gunAudio.hpp"

XT_Wav_Class LaserSound(laser_wav);                   // create an object of type XT_Wav_Class that is used by
                                                      // the dac audio class (below), passing wav data as parameter.
XT_Wav_Class GlitchSound(glitch_wav);                 // create an object of type XT_Wav_Class that is used by
                                                      // the dac audio class (below), passing wav data as parameter.
XT_DAC_Audio_Class DacAudio(DAC_GPIO_PIN, DAC_TIMER); // Create the main player class object.
                                                      // Use GPIO 25, one of the 2 DAC pins and timer 0
TaskHandle_t audioTaskHandle = nullptr;

void PlayAudio(void *parameter)
{
#ifdef AUDIO_DEBUG
    Serial.println("Starting Sound");
#endif

    DacAudio.FillBuffer();
    DacAudio.Play(&LaserSound);
    DacAudio.Play(&GlitchSound, true);

    while (LaserSound.Playing == true)
    {
        DacAudio.FillBuffer();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

#ifdef AUDIO_DEBUG
    Serial.println("Ending Sound");
#endif
    DacAudio.StopAllSounds();

    audioTaskHandle = nullptr;
    // When you're done, call vTaskDelete. Don't forget this!
    vTaskDelete(nullptr);
}

void StopAudio()
{
    DacAudio.StopAllSounds();
}

void StartAudioTask()
{
    if (audioTaskHandle == nullptr)
    {
        xTaskCreatePinnedToCore(
            PlayAudio,
            "Play audio",
            AUDIO_STACK_SIZE,
            nullptr,
            AUDIO_PRIORITY,
            &audioTaskHandle,
            AUDIO_CORE);
    }
}
