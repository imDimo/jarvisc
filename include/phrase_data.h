#ifndef JARVISC_PHRASE_DATA
#define JARVISC_PHRASE_DATA

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

struct phrase {
    // Phrase to trigger this action
    char* phrase = nullptr; 

    // Whether or not the phrase is currently being matched
    bool is_matching = false; 

    // Actions to be run when the phrase is matched
    action* actions = nullptr;
};

// Part of a sentence including a word, its length, and a reference to the next part
struct sentence_part {
    char* word = nullptr;
    int word_length = 0;
    sentence_part* next = nullptr;
};


sentence_part* deallocate_part(sentence_part* part);
void deallocate_sentence(sentence_part* start);

#endif
