/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * These functions produce random-gibberish quasi-words in a quasi-sentence.
 * Random character streams may be conceptually sufficient, but for testing
 * purposes, it's much easier for humans to remember and to distinguish semi-
 * pro-nounceable gibberish like "dokwab neltiegib..." than
 * "f7H%r^&B*5|j6@Mz>\#...".
 */

#include <string>

#include "gibberish.hpp"  // shouldn't need any other project headers


/**
 * \brief Returns a letter for random-gibberish quasi-words in a quasi-sentence.
 *
 * \return A letter character value.
 *
 */
char gibberish::letter(void)
{
    return 'a' + (rand() % ('z'-'a' + 1));
}


/**
 * \brief Returns a vowel for random-gibberish quasi-words in a quasi-sentence.
 *
 * \return A vowel character value.
 *
 */
char gibberish::vowel(void)
{
    char vowels[] = "aeiou";

    return vowels[rand() % 5];
}


/**
 * \brief Returns a consonant for random-gibberish quasi-words in a quasi-sentence.
 *
 * \return A consonant character value.
 *
 */
char gibberish::consonant(void)
{
    char candidate;

    do {
        candidate = letter();
    } while (   candidate == 'a' || candidate == 'e' || candidate == 'i'
             || candidate == 'o' || candidate == 'u');
    return candidate;
}


/**
 * \brief Appends a semi-pronounceable syllable onto a string, stopping before
 *        the end of the string, for random-gibberish quasi-words in a
 *        quasi-sentence.  Returns a pointer to the next open spot in the string.
 *
 * \param[in] string_ptr Pointer to where to put the word.
 *
 * \param[in] stop       Pointer to last character in quasi-sentence.
 *
 * \return    Pointer to first character after the word.
 *
 */
char *gibberish::syllable (char *string_ptr, char *stop)
{
    char *parser;  /* points into string while building it */

    parser = string_ptr;
    if ((rand() % 4) < 3) {
        if (parser < stop) *parser++ = consonant();
        if (parser < stop) *parser++ = vowel();
        if (parser < stop) *parser++ = letter();
    } else {
        if (parser < stop) *parser++ = vowel();
        if (((rand() % 4) < 1) && parser < stop) {
            *parser++ = vowel();
        }
        if (parser < stop) *parser++ = consonant();
    }
    return parser;
}


/**
 * \brief Appends a mostly-pronounceable quasi-word onto a quasi-sentence string,
 *        stopping before the end of the string.  Returns a pointer to the next
 *        open spot in the string.
 *
 * \param[in] initial_cap:  True if the first character should be capitalized.
 *
 * \param[in] string_ptr Pointer to where to put the word.
 *
 * \param[in] stop       Pointer to last character in quasi-sentence.
 *
 * \return    Pointer to first character after the word.
 *
 */
char *gibberish::word (bool initial_cap, char *string_ptr, char *stop)
{
    int syllable_count;
    char *parser;  /* points into string while building it */

    for (syllable_count = 0, parser = string_ptr;
            syllable_count < 4
         && (rand() % 4) >= syllable_count
         && parser < stop;
         syllable_count++) {
        parser = syllable (parser, stop);
    }
    if (initial_cap) {
        *string_ptr -= 'a' - 'A';  /* more or less assumes ASCII */
    }
    return parser;
}


/**
 * \brief Creates a mostly-pronounceable, random-gibberish quasi-sentence,
 *        stopping before the end of the string.
 *
 * \param[in] string_ptr Pointer to beginning of string for quasi-sentence.
 *
 * \param[in] stop       Pointer to last character in quasi-sentence.
 *
 */
void gibberish::sentence (char *string_ptr, char *stop)
{
    char *parser;  /* points into string while building it */
    char punctuation[] = ".?!";

    *stop = '\0';  /* null-terminate the string */
    --stop;
    parser = word (capitalize, string_ptr, stop);
    if (parser < stop) {
        *parser++ = ' ';
    }
    for (;  parser < stop; ) {
        parser = word (dont_capitalize, parser, stop);
        if (parser < stop) {
            *parser++ = ' ';
        }
    }
    parser--;  
    if (*parser == ' ') {
        *parser = vowel();  // just to not have a blank at the end
    }
    *stop = punctuation[rand() % 3];
}


/**
 * \brief Chooses a gibberish-sentence length.
 *
 */
int gibberish::pick_sentence_len (void)
{
    return min_literal_data_len + (rand() % literal_data_len_span);
}


/**
 * \brief Constructor for gibberish object.
 *
 */
gibberish::gibberish (void)
{
    // Nothing to set up.
}


/**
 * \brief Destructor for gibberish object.
 *
 */
gibberish::~gibberish (void)
{
    // Nothing to tear down.
}
