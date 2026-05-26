/* File: OnlyConnect.cpp
 *
 * Anything noteworthy in the code:
 * Yes - a question here: any ways to further simplify my code?
 *
 * Puzzle: SNGJNCH (hint: name of a super famous Korean pianist!)
 */
#include "OnlyConnect.h"
#include "strlib.h"
using namespace std;

string onlyConnectize(string phrase) {
    int len = phrase.length();
    if (len == 0) {
        return "";
    } else if (len == 1) {
        char c = stringToChar(phrase);
        c = toupper(c);
        if (isalpha(c) && c != 'A' && c != 'E' && c != 'I' && c != 'O' && c != 'U') {
            return charToString(c);
        } else {
            return "";
        }
    } else {
        return onlyConnectize(phrase.substr(0, len - 1)) + onlyConnectize(phrase.substr(len - 1));
    }
}






/* * * * * * Unit Tests Curated and Written by Sunny Zhao * * * * * */
#include "GUI/SimpleTest.h"

STUDENT_TEST("Handles single-character inputs.") {
    EXPECT_EQUAL(onlyConnectize("A"), "");
    EXPECT_EQUAL(onlyConnectize("Q"), "Q");
}

STUDENT_TEST("Converts lower-case to upper-case.") {
    EXPECT_EQUAL(onlyConnectize("lowercase"), "LWRCS");
    EXPECT_EQUAL(onlyConnectize("uppercase"), "PPRCS");
}

STUDENT_TEST("Digits are not added.") {
    EXPECT_EQUAL(onlyConnectize("1"), "");
    EXPECT_EQUAL(onlyConnectize("323T"), "T");
}

STUDENT_TEST("Handles empty strings.") {
    EXPECT_EQUAL(onlyConnectize(""), "");
}

STUDENT_TEST("Vowels are not added.") {
    EXPECT_EQUAL(onlyConnectize("QWERTY"), "QWRTY");
    EXPECT_EQUAL(onlyConnectize("graphic"), "GRPHC");
}

STUDENT_TEST("Punctuations and spaces are not added.") {
    EXPECT_EQUAL(onlyConnectize("(oh!)wow"), "HWW");
    EXPECT_EQUAL(onlyConnectize("a tie"), "T");
}






