#ifndef JARVISC_ASR_HANDLER
#define JARVISC_ASR_HANDLER

#include "definitions.h"

#include "phrase_data.h"

// Use system install of april-asr if it was found during compilation
#ifdef APRILASR_INSTALLED
#include <april-asr/april_api.h>
#else
#include "../external/source/april-asr/april_api.h"
#endif

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
