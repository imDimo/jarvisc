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

    // AsrHandler* asr_handler = AsrHandler::getHandler();
    // asr_sentence_data* sentence_data = AsrHandler::getHandler()->sentence_data;

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

    char recording_data[BUFFER_SIZE];
    ssize_t r;
    for(;;) {
        r = read(STDIN_FILENO, recording_data, BUFFER_SIZE);

        if (r == -1) {
            aas_flush(session);
            break;
        } 
        else if (r <= 0) {
            continue;
        }

        aas_feed_pcm16(session, (short *)recording_data, r/2);
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

    aas_free(session);
    aam_free(model);

    std::cout << "Exiting!" << std::endl;
    return 0;
}
