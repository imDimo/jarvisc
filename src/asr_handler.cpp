#include "../include/asr_handler.h"

#include <cassert>
#include <iostream>
#include <stdio.h>
#include <stdbool.h>

#include "../include/phrase_data.h"

void reset_sentence_data(asr_sentence_data& data) {

    data.sentence.clear();
    data.prev_sentence_length = 0;
    data.sentence_start_offset = 0;
}

bool is_punctuation(char c) {
    switch (c) {
        case '.': case '!': case '?':
            return true;
    }

    return false;
}

bool is_other_special(char c) {
    switch (c) {
        case ',': case '\'': case '$':
        return true;
    }

    return false;
}

void handler(void *handler_data, AprilResultType result, size_t count, const AprilToken *tokens) {
    asr_sentence_data* data = (asr_sentence_data*)handler_data;
    data->result_type = result;
    data->token_count = count;

    delete[] data->tokens;
    data->tokens = new AprilToken[count];
    for (int t = 0; t < count; t++)
        data->tokens[t] = tokens[t];
}

void process_tokens(asr_sentence_data& data) {
    int start_token = 0;

    // Get number of tokens in the sentence up until the most recent word
    // We want to skip as many pre-existing tokens as possible, but the asr library may alter/correct the last word in the sentence, so we go back to fetch those changes
    // for (auto part = std::next(data.sentence.begin(), data.sentence_start_offset); part != std::prev(data.sentence.end()); part++)
        // start_token += (*part).num_tokens;

    for (auto part = std::next(data.sentence.begin(), data.sentence_start_offset); part != std::prev(data.sentence.end()); part++)
        start_token += (*part).num_tokens;

    // If the index of the start token is greater than the number of tokens provided by the asr, then the token array was reset
    // Apply an offset to ignore words no longer included in the tokens list and continue as if the sentence is empty
    // We also pop the last word in the list because it will become the first word of the new sentence
    if (start_token > data.token_count) {
        data.prev_sentence_length = 0;
        data.sentence_start_offset = data.sentence.size();
        start_token = 0;
    }

    std::string previous_last_word = "";

    if (data.sentence.size() - data.sentence_start_offset > 0) {
        // Save the text of the last word for later comparison
        previous_last_word = data.sentence.back().text;
        data.sentence.pop_back();
    }

    // Create words from tokens and add them to the sentence
    for (int t = start_token; t < data.token_count; t++) {
        const char* curr_token = data.tokens[t].token;
        int c = 0;

        // Detect new word
        if (curr_token[0] == ' ') {
            sentence_part part{ "", 1 };
            data.sentence.push_back(part);

            // Skip space at start of word
            c++;
        }
        else {
            data.sentence.back().num_tokens++;

            if (is_punctuation(curr_token[0])) {
                break;
            }
        }

        char temp_word[MAX_WORD_LENGTH];
        int char_index = 0;
        while (curr_token[c] != '\0' && char_index < MAX_WORD_LENGTH) {
            if (!is_other_special(curr_token[c])) {
                temp_word[char_index] = curr_token[c];
                char_index++;
            }

            c++;
        }

        temp_word[char_index] = '\0';

        data.sentence.back().text += temp_word;
    }

    switch(data.result_type){
        case APRIL_RESULT_RECOGNITION_FINAL:

            reset_sentence_data(data);

            return;
        case APRIL_RESULT_RECOGNITION_PARTIAL:
            break;

        case APRIL_RESULT_SILENCE:

            reset_sentence_data(data);

            return;
        default:
            // assert(false);
            return;
    }

    data.prev_sentence_length = data.sentence.size();
    data.updated = data.prev_sentence_length != data.sentence.size() || previous_last_word != data.sentence.back().text;

    // for (int t= 0 ; t < data.token_count; t++)
    //     std::cout << data.tokens[t].token;
    // std::cout << std::endl;
}
