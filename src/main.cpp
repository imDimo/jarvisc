/*
 * Copyright (C) 2022 abb128
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <getopt.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <ostream>
#include <time.h>

#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#include <BaseTsd.h>
#define STDIN_FILENO 0
typedef SSIZE_T ssize_t;
#endif

#include "../include/definitions.h"

#include "../include/asr_handler.h"
#include "../include/phrase_data.h"

#include "../include/recording.h"

int main (int argc, char *argv[]) {
    struct jarvisc_options {
        char* model_path;
        int logging_enabled;
    } args{};

    static struct option long_options[] = {
        { "model",  required_argument,  0,  'm' },
        { 0, 0, 0, 0 }
    };

    for (;;) {
        int opt;
        int option_index;
        opt = getopt_long(argc, argv, "m:", long_options, &option_index);

        if (opt == -1)
            break;

        switch (opt) {
            case 0:
                break;

            case 'm':
                std::cout << "Model path set to " << optarg << std::endl;
                args.model_path = optarg;
                break;

            case '?':
                return -1;
            default:
                abort();
        }
    }

    // In the start of our program we should call aam_api_init.
    // This should only be called once.
    aam_api_init(APRIL_VERSION);

    // Next we should load the model. The model by itself doesn't allow us
    // to do much except for get the metadata. If loading the model
    // fails, NULL is returned.
    AprilASRModel model = aam_create_model(args.model_path);
    if(model == NULL){
        std::cout << "Loading model" << args.model_path <<  " failed!\n";
        return 1;
    }
    
    size_t model_sample_rate = aam_get_sample_rate(model);
    std::cout << "Model name: " << aam_get_name(model) << "\n";
    std::cout << "Model desc: " << aam_get_description(model) << "\n";
    std::cout << "Model lang: " << aam_get_language(model) << "\n";
    std::cout << "Model samplerate: " << model_sample_rate << "\n\n";

    asr_sentence_data sentence_data{};
    
    AprilConfig config{};
    config.handler = handler;
    config.userdata = (void*)&sentence_data;
    
    // By default, the session runs in synchronous mode. If you want async
    // processing, you may choose to set it to APRIL_CONFIG_FLAG_ASYNC_RT_BIT
    // here.
    config.flags = APRIL_CONFIG_FLAG_ZERO_BIT;
    AprilASRSession session = aas_create_session(model, config);
    
    PaError pa_err = Pa_Initialize();
    if (pa_err != paNoError) {
        std::cout << "Error while initializing PortAudio: " << Pa_GetErrorText(pa_err) << std::endl;
        exit(1);
    }

    PaStream* pa_stream;
    pa_err = Pa_OpenDefaultStream(&pa_stream, 1, 0, paInt16, aam_get_sample_rate(model), BUFFER_SIZE, NULL, NULL);
    
    if (pa_err != paNoError) {
        std::cout << "Error while opening PortAudio stream: " << Pa_GetErrorText(pa_err) << std::endl;
        exit(2);
    }
    
    pa_err = Pa_StartStream(pa_stream);
    
    if (pa_err != paNoError) {
        std::cout << "Error while starting PortAudio stream: " << Pa_GetErrorText(pa_err) << std::endl;
        exit(3);
    }

    short* input_data = new short[BUFFER_SIZE];
    for(;;) {
        pa_err = Pa_ReadStream(pa_stream, input_data, BUFFER_SIZE);

        if (pa_err != paNoError) {
            std::cout << "Error while reading from PortAudio stream: " << Pa_GetErrorText(pa_err) << std::endl;
            exit(3);
        }
        
        aas_feed_pcm16(session, input_data, BUFFER_SIZE);
        process_tokens(sentence_data);

        if (sentence_data.updated) {
            sentence_data.updated = false;

            for (auto part : sentence_data.sentence) {
                std::cout << part.text << ' ';
            }
            std::cout << std::endl;
        }

        // TODO: Scan sentence for matching phrases
    }

    pa_err = Pa_StopStream(pa_stream);
    if (pa_err != paNoError) {
        std::cout << "Error while stopping PortAudio stream: " << Pa_GetErrorText(pa_err) << std::endl;
        exit(4);
    }

    pa_err = Pa_CloseStream(pa_stream);
    if (pa_err != paNoError) {
        std::cout << "Error while stopping PortAudio stream: " << Pa_GetErrorText(pa_err) << std::endl;
        exit(4);
    }

    pa_err = Pa_Terminate();
    if( pa_err != paNoError )
        std::cout << "Error while terminating PortAudio: " << Pa_GetErrorText(pa_err) << std::endl;

    aas_free(session);
    aam_free(model);

    std::cout << "Exiting!" << std::endl;
    return 0;
}
