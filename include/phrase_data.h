#ifndef JARVISC_PHRASE_DATA
#define JARVISC_PHRASE_DATA

#include "definitions.h"

/**
 * Types of actions
 *
 * COMMAND: A binary/executable to be run
 *
 * HOTKEY: A keyboard input to be triggered
 *
 * OSC: An OSC value to be sent over the local system
 */
enum ACTION_TYPE {
    COMMAND, HOTKEY, OSC
};

struct action {
    // The type of aaction to be run
    // Primarily alters how the data field is interpreted
    ACTION_TYPE action_type;

    // Data field's contents depends on the type of action
    //
    // For COMMAND, data will be the name of a binary or executable followed by any desired arguments.
    //
    // For HOTKEY, data will be one or more keyboard characters. Actions are executed sequentially, so muliple HOTKEY actions can be ordered to perform a macro.
    //
    // For OSC, data will contain the provided OSC data.
    char* data; 
};

// Part of a sentence including a word, its length, and a reference to the next part
struct sentence_part {
    // Characters of the word held in this part of the sentence
    char text[MAX_WORD_LENGTH];
    // char* text;
    // Reference to the next part of the sentence
    sentence_part* next;
    // Reference to the previous part of the sentence
    sentence_part* prev;
    // Number of characters in the word
    int length;
    // Number of tokens used to construct this word, including tokens for punctuation and other removed symbols
    int num_tokens;
};

struct phrase {
    // Phrase to trigger this action
    sentence_part* phrase; 
    // char* phrase; 

    // Whether or not the phrase is currently being matched
    bool is_matching; 

    // Actions to be run when the phrase is matched
    action* actions;
};

sentence_part* deallocate_part(sentence_part* part);
sentence_part* deallocate_part_end(sentence_part* part);
void deallocate_parts_from_start(sentence_part* start);

#endif
