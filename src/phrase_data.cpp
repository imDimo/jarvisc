#include "../include/phrase_data.h"

sentence_part* deallocate_part(sentence_part* part) {
    // delete[] part->text;
    // part->text = nullptr;

    sentence_part* next = part->next;
    delete part;
    part = nullptr;

    return next;
}

sentence_part* deallocate_part_end(sentence_part* part) {
    // delete[] part->text;
    // part->text = nullptr;

    sentence_part* prev = part->prev;
    delete part;
    part = nullptr;

    return prev;
}

void deallocate_parts_from_start(sentence_part* start) {
    sentence_part* current_part = start;

    while (current_part != nullptr) {
        current_part = deallocate_part(current_part);
    }
}
