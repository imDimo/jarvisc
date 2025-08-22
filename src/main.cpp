// For basic live captioning of desktop audio, run it like so:
// parec --format=s16 --rate=16000 --channels=1 --latency-ms=100 --device=@DEFAULT_MONITOR@ | -m /path/to/model.april

#include <getopt.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <ostream>
#include <time.h>
#include <errno.h>

#include "../external/source/april-asr/april_api.h"

#include "../include/phrase_data.h"

#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#include <BaseTsd.h>
#define STDIN_FILENO 0
typedef SSIZE_T ssize_t;
#endif

#define BUFFER_SIZE 1024
#define MAX_WORD_LENGTH 32

// Data structure holding all information passed into callback
struct {
    // Track position in sentence currently being built
    sentence_part* sentence_start = nullptr;
    sentence_part* sentence_end = nullptr;
    
    phrase* phrases = nullptr;
    int prev_token_count = 0;
} program_data;

bool is_punctuation(char c) {
    switch (c) {
        case '.': case '!': case '?':
            return true;
    }

    return false;
}

// This callback function will get called every time a new result is decoded.
// It's passed into the AprilConfig along with the userdata pointer.
void handler(void *handler_data, AprilResultType result, size_t count, const AprilToken *tokens) {
    assert(handler_data == &program_data);
    
    if (program_data.sentence_start != nullptr && program_data.sentence_end != nullptr) {

        for (int t = program_data.prev_token_count; t < count; t++) {

            const char* curr_token = tokens[t].token;
            
            // Not all models set the SENTENCE_END_BIT, so a manual check is required for compatibility
            if (/*tokens[t].flags && APRIL_TOKEN_FLAG_SENTENCE_END_BIT != 0 || */is_punctuation(curr_token[0])) {
                continue;
            }
            
            bool new_word = false;

            if (tokens[t].flags && APRIL_TOKEN_FLAG_WORD_BOUNDARY_BIT != 0)
                new_word = true;

            std::cout << curr_token;
            std::flush(std::cout);

            sentence_part* curr_word;
            sentence_part* prev_word;
            
            int c = 0;
            if (new_word) {
                curr_word = new sentence_part { 0 };
                curr_word->word = new char[MAX_WORD_LENGTH];

                prev_word = program_data.sentence_end;
                program_data.sentence_end->next = curr_word;
                program_data.sentence_end = curr_word;

                // Skip special character in character copy loop
                c++;
            }
            else {
                // Append to existing word fragment
                curr_word = program_data.sentence_end;
            }

            while (curr_token[c] != '\0' && curr_word->word_length < MAX_WORD_LENGTH) {
                if (curr_token[c] != ',' && curr_token[c] != '\'') {
                    curr_word->word[curr_word->word_length] = curr_token[c];
                    curr_word->word_length++;
                }

                c++;
            }

            curr_word->word[curr_word->word_length] = '\0';
        }

        sentence_part* word = program_data.sentence_start;

        switch(result){
            case APRIL_RESULT_RECOGNITION_FINAL: 
                //std::cout << '@';
                // std::cout << "\n[" << data.sentence << "]\n";

                break;
            case APRIL_RESULT_RECOGNITION_PARTIAL:

                //std::cout << "- ";
                //std::cout << '\n' << data.sentence << '\n';
                break;
            case APRIL_RESULT_SILENCE:
                
                /*
                std::cout << "\n[ ";

                while (word->next != nullptr) {

                    word = word->next;
                    std::cout << word->word << ' ';
                }
                std::cout << "]\n";
                */
                std::cout << std::endl;
                deallocate_sentence(program_data.sentence_start);
                
                program_data.sentence_start = new sentence_part { 0 };
                program_data.sentence_start->word = new char[MAX_WORD_LENGTH];
                program_data.sentence_end = program_data.sentence_start;
                program_data.prev_token_count = 0;

                return;

            default:
                assert(false);
                return;
        }

        program_data.prev_token_count = count;
    }
    else {
        delete program_data.sentence_start;
        delete program_data.sentence_end;

        program_data.sentence_start = new sentence_part { 0 };
        program_data.sentence_start->word = new char[MAX_WORD_LENGTH];
        program_data.sentence_end = program_data.sentence_start;
        program_data.prev_token_count = 0;
    }
}

int main (int argc, char *argv[]) {
    struct jarvisc_options {
        char* model_path;
        int logging_enabled;
    } args = { 0 };

    static struct option long_options[] = {
        { "log",    no_argument,        &args.logging_enabled,    1},
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

    AprilConfig config = { 0 };
    config.handler = handler;
    config.userdata = (void*)&program_data;

    // By default, the session runs in synchronous mode. If you want async
    // processing, you may choose to set it to APRIL_CONFIG_FLAG_ASYNC_RT_BIT
    // here.
    config.flags = APRIL_CONFIG_FLAG_ZERO_BIT;

    AprilASRSession session = aas_create_session(model, config);

    if(argv[1][0] == '-' && argv[1][1] == 'm') {
        // Reading stdin mode. It's assumed that the input data is pcm16 audio,
        // sampled in the model's sample rate.
        // You can achieve this on Linux like this:
        // $ parec --format=s16 --rate=16000 --channels=1 --latency-ms=100 | ./main -m /path/to/model.april

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

            // TODO: Scan sentence to match phrases
            program_data.sentence_start;
        }
    } 
    
    aas_free(session);
    aam_free(model);

    return 0;
}
