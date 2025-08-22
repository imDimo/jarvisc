#include "../include/phrase_data.h"

sentence_part* deallocate_part(sentence_part* part) {
    delete[] part->word;
    part->word = nullptr;

    sentence_part* next = part->next;
    delete part;
    part = nullptr;

    return next;
}

void deallocate_sentence(sentence_part* start) {
    sentence_part* current_part = start;

    while (current_part != nullptr) {
        current_part = deallocate_part(current_part);
    }
}
