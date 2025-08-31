#ifndef JARVISC_ASR_HANDLER
#define JARVISC_ASR_HANDLER

#include "definitions.h"

#include <list>

#include "phrase_data.h"

// Use system install of april-asr if it was found during compilation
#ifdef APRILASR_INSTALLED
#include <april-asr/april_api.h>
#else
#include "../external/source/april-asr/april_api.h"
#endif

struct asr_sentence_data {
    std::list<sentence_part> sentence;
    int token_count;
    int prev_sentence_length;
    AprilResultType result_type;
    AprilToken* tokens;
    int sentence_start_offset;
    bool updated;
};

void handler(void *handler_data, AprilResultType result, size_t count, const AprilToken *tokens);
void process_tokens(asr_sentence_data& data);

#endif
