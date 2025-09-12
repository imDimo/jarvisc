#ifndef JARVISC_PORTAUDIO
#define JARVISC_PORTAUDIO

// Use system install of portaudio if it was found during compilation
#ifdef PORTAUDIO_INSTALLED
#include <portaudio.h>
#else
#include "../external/source/PortAudio/include/portaudio.h"
#endif

struct pa_audio_data {

};

int pa_callback(const void *input, void *output, unsigned long frameCount, 
                const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
#endif
