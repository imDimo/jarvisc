#ifndef JARVISC_ASR_HANDLER
#define JARVISC_ASR_HANDLER

#include "../include/phrase_data.h"
#include "../external/source/april-asr/april_api.h"
#include <mutex>

#define MAX_WORD_LENGTH 32

struct asr_sentence_data {
    sentence_part* sentence_start;
    sentence_part* sentence_end;
    int token_count;
    int prev_token_count;
    AprilResultType result_type;
    AprilToken* tokens;
};

void handler(void *handler_data, AprilResultType result, size_t count, const AprilToken *tokens);
void process_tokens(asr_sentence_data* data);

#endif
