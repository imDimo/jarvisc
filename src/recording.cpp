#include "../include/recording.h"

int pa_callback(
    const void *input, void *output, unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void *userData)
{
    pa_audio_data *data = (pa_audio_data*)userData;

    return 0;
}
