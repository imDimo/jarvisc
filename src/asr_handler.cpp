#include "../include/asr_handler.h"

#include <cassert>
#include <iostream>
#include <stdio.h>
#include <stdbool.h>

#include "../include/phrase_data.h"

bool is_punctuation(char c) {
    switch (c) {
        case '.': case ',': case '!': case '?': case '\'':
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

void process_tokens(asr_sentence_data* data) {

    if (data->sentence_start == nullptr || data->sentence_end == nullptr) {
        delete data->sentence_start;
        delete data->sentence_end;

        data->sentence_start = new sentence_part();
        data->sentence_end = data->sentence_start;
        data->prev_token_count = 0;
    }

    int start_token = 0;

    sentence_part* part = data->sentence_start;

    // Get number of tokens leading up to (but not including) the last part of the sentence
    while (part->next != nullptr) {
        start_token += part->num_tokens;
        part = part->next;
    }

    // Strip the last part in the sentence if it is not the only part
    if (data->sentence_start != data->sentence_end) {
        data->sentence_end = deallocate_part_end(data->sentence_end);
        data->sentence_end->next = nullptr;
    }

    // Previous tokens may change, so we set the token counter before the last word to catch those changes
    for (int t = start_token; t < data->token_count; t++) {
        const char* curr_token = data->tokens[t].token;
        sentence_part* curr_word = data->sentence_end;

        bool is_new_word = false;
        if (curr_token[0] == ' ')
            is_new_word = true;
        else
            curr_word->num_tokens++;


        if (is_punctuation(curr_token[0]))
            continue;

        int c = 0;
        if (is_new_word) {
            curr_word = new sentence_part();
            curr_word->prev = data->sentence_end;
            data->sentence_end->next = curr_word;
            data->sentence_end = curr_word;

            curr_word->num_tokens++;

            // Skip space at start of word
            c++;
        }

        while (curr_token[c] != '\0' && curr_word->length < MAX_WORD_LENGTH) {
            if (curr_token[c] != ',' && curr_token[c] != '\'') {
                curr_word->text[curr_word->length] = curr_token[c];
                curr_word->length++;
            }

            c++;
        }

        curr_word->text[curr_word->length] = '\0';
    }

    sentence_part* word = data->sentence_start;

    switch(data->result_type){
        case APRIL_RESULT_RECOGNITION_FINAL: 
            deallocate_parts_from_start(data->sentence_start);

            data->sentence_start = new sentence_part();
            data->sentence_end = data->sentence_start;
            data->prev_token_count = 0;
            std::cout << '\n';
            //std::cout << '@';
            // std::cout << "\n[" << data->sentence << "]\n";

            break;
        case APRIL_RESULT_RECOGNITION_PARTIAL:

            //std::cout << "- ";
            //std::cout << '\n' << data->sentence << '\n';
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

            // std::cout << std::endl;
            deallocate_parts_from_start(data->sentence_start);

            data->sentence_start = new sentence_part();
            data->sentence_end = data->sentence_start;
            data->prev_token_count = 0;

            return;

        default:
            // assert(false);
            return;
    }

    data->prev_token_count = data->token_count;
}
