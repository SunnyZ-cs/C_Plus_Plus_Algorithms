#include "SpeakingRecursian.h"
#include "strlib.h"
using namespace std;

/* Global constants declaration:
 * Here we store the vowels and consonants into two lists of strings. They are not stored with char
 * as the data type because it's much simpler to carry out concatenation when all variables are of
 * the type string.
 */
const Vector<string> vowels = {"e", "i", "u"};
const Vector<string> consnts = {"b", "k", "n", "r", "s", "'"};

/* Below is a simple (and general) iterative function that helps to concatenate strings together */
Vector<string> loopHelper(const Vector<string>& lst1,
                          const Vector<string>& lst2) {
    Vector<string> output;
    for (string elem1: lst1) {
        for (string elem2: lst2) {
            output += elem1 + elem2;
        }
    }
    return output;
}

Vector<string> allRecursianWords(int numSyllables) {

    /* Error case elimination*/
    if (numSyllables < 0) {
        error("Number of syllables is negative.");
    }

    /* Base case:
     * By definition, when the number of syllables is 0, the empty string itself is the word
     * with 0 syllables.
     */
    if (numSyllables == 0) {
        return {""};
    }

    /* Recursive case:
     * (1) Step one: add consonants to the end of strings made for the number of syllables that's
     * exactly one degree smaller than the current given number of syllables.
     */
    Vector<string> wordsPlusCon = loopHelper(allRecursianWords(numSyllables - 1), consnts);

    /* (2) Step two: add vowels to the end of strings in the list wordsPlusCon which all have already
     * been attached with a consonant at the end.
     */
    Vector<string> wordsPlusVow = loopHelper(wordsPlusCon, vowels);

    /* One more step here if the number of syllables is one, in order to address the exception where the first
     * syllable in a word can consist of a single vowel.
     */
    if (numSyllables == 1) {
        return wordsPlusVow + vowels;
    }
    return wordsPlusVow;
}





/* * * * * Unit tests curated by Sunny Zhao * * * * */
#include "GUI/SimpleTest.h"
#include "GUI/TextUtils.h"
#include "set.h"

STUDENT_TEST("allRecursianWords works in simple cases.") {
    EXPECT_EQUAL(allRecursianWords(0), {""});
    EXPECT_ERROR(allRecursianWords(-1));
    EXPECT_ERROR(allRecursianWords(-137));
}

STUDENT_TEST("allRecursianWords works for length 1.") {
    auto wordsUnsorted = allRecursianWords(1);

    /* Sort the words using a set. This will also eliminate duplicates. */
    Set<string> words;
    for (string word: wordsUnsorted) {
        /* All one-syllable words have length one or two. */
        EXPECT(word.length() == 1 || word.length() == 2);

        words += word;
    }

    /* Should be the same number of words once we sort them. */
    EXPECT_EQUAL(wordsUnsorted.size(), words.size());

    Set<string> expected = {
        "'e", "'i", "'u", "be", "bi", "bu", "e", "i",
        "ke", "ki", "ku", "ne", "ni", "nu", "re", "ri",
        "ru", "se", "si", "su", "u"
    };

    EXPECT_EQUAL(words, expected);
}

STUDENT_TEST("allRecursianWords has the right quantities of words.") {
    EXPECT_EQUAL(allRecursianWords(0).size(), 1);
    EXPECT_EQUAL(allRecursianWords(1).size(), 21);
    EXPECT_EQUAL(allRecursianWords(2).size(), 378);
    EXPECT_EQUAL(allRecursianWords(3).size(), 6804);
    EXPECT_EQUAL(allRecursianWords(4).size(), 122472);
}

namespace {
    bool isConsonant(char ch) {
        return ch == 'b' || ch == 'k' || ch == 'n' || ch == 'r' || ch == 's' || ch == '\'';
    }
    bool isVowel(char ch) {
        return ch == 'e' || ch == 'i' || ch == 'u';
    }
}

STUDENT_TEST("allRecursianWords produces words consisting of consonants and vowels.") {
    /* There are too many words here to check the answer against a reference - and it's
     * infeasible to manually create a reference. Instead, we will generate a list of
     * words and confirm that each one is either a consonant or a vowel. If we see
     * anything else, we know the answer is wrong.
     *
     * This won't catch everything, though. It's entirely possible to have a word made
     * entirely of vowels pass, or a word of way too many consonants pass. You will
     * need to write your own test cases to cover cases not detected here.
     */
    auto words = allRecursianWords(4);
    EXPECT_NOT_EQUAL(words.size(), 0);

    for (string word: words) {
        for (char ch: word) {
            EXPECT(isConsonant(ch) || isVowel(ch));
        }
    }
}

STUDENT_TEST("The characters in the words allRecursianWords produces should have alternating consonants and vowels.") {
    auto words = allRecursianWords(4);
    EXPECT_NOT_EQUAL(words.size(), 0);

    for (string word: words) {
        for (int i = 1; i < word.length(); i++) {
            EXPECT((isConsonant(word[i]) && isVowel(word[i - 1])) || (isConsonant(word[i - 1]) && isVowel(word[i])));
        }
    }
}
