#include "Demos/AudioSystem.h"
#include "StringInstrument.h"
#include "error.h"
using namespace std;

/* The magic numbers amplitude and the decay rate of the sound wave are declared here. */
const Sample AMPLITUDE = 0.05;
const Sample DECAYRATE = 0.995;

/* The constructor initializes the waveform array, writes down its length for later and
 * sets the cursor to position 0, at the start of the buffer.
 */
StringInstrument::StringInstrument(double frequency) {
    if (frequency <= 0) {
        error("frequency cannot be zero or negative");
    }

    length = AudioSystem::sampleRate() / frequency;

    if (length == 0 || length == 1) {
        error("resulting array has size 0 or 1");
    }

    waveform = new Sample[length];

    for (int i = 0; i < length; i++) {
        waveform[i] = 0;
    }

    cursor = 0;
}

/* Destructor that cleans up memory used in constructor. */
StringInstrument::~StringInstrument() {
    delete[] waveform;
}

/* This member function pluck fills the first half of the array to +0.05
 * and the second half to -0.05.
 */
void StringInstrument::pluck() {
    int half = length / 2;
    for (int i = 0; i < length; i++) {
        if (i < half) {
            waveform[i] = AMPLITUDE;
        } else {
            waveform[i] = -1 * AMPLITUDE;
        }
    }

    cursor = 0;
}

/* This member function returns the next sound sample and updates the waveform
 * buffer and cursor position. */
Sample StringInstrument::nextSample() {
    Sample result = waveform[cursor];
    int nextIdx = (cursor + 1) % length;
    Sample next = waveform[nextIdx];
    waveform[cursor] = ((result + next) / 2) * DECAYRATE;
    cursor = nextIdx;
    return result;
}

/* * * * * Unit tests curated by Sunny Zhao * * * * */

#include "GUI/SimpleTest.h"

STUDENT_TEST("Milestone 2: Constructor does the correct round-down when computing length.") {
    AudioSystem::setSampleRate(10);

    StringInstrument instrument(3);

    /* 10/3 = 3.33333, which rounds down to the integer 3. */
    EXPECT_EQUAL(instrument.length, 3);
}

STUDENT_TEST("Milestone 3: pluck works with odd array length.") {
    AudioSystem::setSampleRate(3);

    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);
    EXPECT_EQUAL(instrument.length, 3);

    instrument.pluck();
    EXPECT_EQUAL(instrument.waveform[0], +0.05);
    EXPECT_EQUAL(instrument.waveform[1], -0.05);
    EXPECT_EQUAL(instrument.waveform[2], -0.05);
}

STUDENT_TEST("Milestone 4: nextSample works for odd array length.") {
    AudioSystem::setSampleRate(3);

    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);
    EXPECT_EQUAL(instrument.length, 3);

    instrument.pluck();
    EXPECT_EQUAL(instrument.nextSample(), +0.05);
    EXPECT_EQUAL(instrument.waveform[0], 0.0);
    EXPECT_EQUAL(instrument.waveform[1], -0.05);
    EXPECT_EQUAL(instrument.waveform[2], -0.05);
}

STUDENT_TEST("Milestone 2: Waveform array initialized correctly.") {
    /* Change the sample rate to 3, just to make the numbers come out nice. */
    AudioSystem::setSampleRate(3);

    /* Create a string that vibrates at 1hz. This is well below the human hearing
     * threshold and exists purely for testing purposes.
     */
    StringInstrument instrument(1);

    /* Make sure something was allocated. */
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);

    /* Length should be 3 / 1 = 3. */
    EXPECT_EQUAL(instrument.length, 3);

    /* All entries should be zero. */
    EXPECT_EQUAL(instrument.waveform[0], 0);
    EXPECT_EQUAL(instrument.waveform[1], 0);
    EXPECT_EQUAL(instrument.waveform[2], 0);
}

STUDENT_TEST("Milestone 2: Constructor reports errors on bad inputs.") {
    /* To make the math easier. */
    AudioSystem::setSampleRate(10);

    EXPECT_ERROR(StringInstrument error(-1));  // Negative frequency
    EXPECT_ERROR(StringInstrument error(0));   // Zero frequency
    EXPECT_ERROR(StringInstrument error(10));  // Array would have length 1
    EXPECT_ERROR(StringInstrument error(100)); // Array would have length 0

    /* But we shouldn't get errors for good values. */
    StringInstrument peachyKeen(1);
    EXPECT_NOT_EQUAL(peachyKeen.waveform, nullptr);
}

STUDENT_TEST("Milestone 2: Constructor sets cursor to position 0.") {
    AudioSystem::setSampleRate(10);

    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);

    EXPECT_EQUAL(instrument.cursor, 0);
}

STUDENT_TEST("Milestone 3: pluck does not allocate a new array.") {
    AudioSystem::setSampleRate(10);

    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);

    /* Plucking the string should change the contents of the array, but not
     * which array we're pointing at.
     */
    Sample* oldArray = instrument.waveform;
    instrument.pluck();

    EXPECT_EQUAL(instrument.waveform, oldArray);
}

STUDENT_TEST("Milestone 3: pluck sets values to +0.05 and -0.05.") {
    AudioSystem::setSampleRate(4);

    /* 4 samples per sec / 1Hz = 4 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);
    EXPECT_EQUAL(instrument.length, 4);

    instrument.pluck();
    EXPECT_EQUAL(instrument.waveform[0], +0.05);
    EXPECT_EQUAL(instrument.waveform[1], +0.05);
    EXPECT_EQUAL(instrument.waveform[2], -0.05);
    EXPECT_EQUAL(instrument.waveform[3], -0.05);
}

STUDENT_TEST("Milestone 3: pluck resets the cursor.") {
    AudioSystem::setSampleRate(4);

    /* 4 samples per sec / 1Hz = 4 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);

    /* Invasively move the cursor forward. This is called an "invasive"
     * test because it manipulates internal state of the type we're
     * testing, rather than just using the interface.
     */
    instrument.cursor = 3;
    instrument.pluck();
    EXPECT_EQUAL(instrument.cursor, 0);
}

STUDENT_TEST("Milestone 4: nextSample works if pluck not called.") {
    AudioSystem::setSampleRate(10);

    /* 10 samples per sec / 1Hz = 10 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);
    EXPECT_EQUAL(instrument.length, 10);

    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument.nextSample(), 0);
    EXPECT_EQUAL(instrument.cursor, 4);
}

STUDENT_TEST("Milestone 4: nextSample updates waveform array.") {
    AudioSystem::setSampleRate(4);

    /* 4 samples per sec / 1Hz = 4 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);
    EXPECT_EQUAL(instrument.length, 4);

    instrument.pluck();
    EXPECT_EQUAL(instrument.nextSample(), +0.05);
    EXPECT_EQUAL(instrument.nextSample(), +0.05);
    EXPECT_EQUAL(instrument.nextSample(), -0.05);
    EXPECT_EQUAL(instrument.cursor, 3);

    /* The first array value is the average of +0.05 and +0.05, scaled by 0.995.
     * The two values are the same, so we should get back +0.05 scalled by 0.995.
     */
    EXPECT_EQUAL(instrument.waveform[0], +0.05 * 0.995);

    /* The next array value is the average of +0.05 and -0.05, scaled by 0.995.
     * This is exactly zero.
     */
    EXPECT_EQUAL(instrument.waveform[1], 0.0);

    /* The next array value is the average of -0.05 and -0.05, scaled by 0.995.
     * As with the first entry, this is -0.05 scaled by 0.995.
     */
    EXPECT_EQUAL(instrument.waveform[2], -0.05 * 0.995);
}

STUDENT_TEST("Milestone 4: nextSample wraps around properly.") {
    AudioSystem::setSampleRate(2);

    /* 2 samples per sec / 1Hz = 2 samples. */
    StringInstrument instrument(1);
    EXPECT_NOT_EQUAL(instrument.waveform, nullptr);
    EXPECT_EQUAL(instrument.length, 2);

    /* Pluck the string, forming the array [+0.05, -0.05] */
    instrument.pluck();

    /* Read two samples, which should be +0.05 and -0.05. */
    EXPECT_EQUAL(instrument.nextSample(), +0.05);
    EXPECT_EQUAL(instrument.cursor, 1);
    EXPECT_EQUAL(instrument.nextSample(), -0.05);
    EXPECT_EQUAL(instrument.cursor, 0);

    /* The first array value is the average of +0.05 and -0.05, scaled by 0.995.
     * This is zero.
     */
    EXPECT_EQUAL(instrument.waveform[0], 0.0);

    /* The next array value is the average of -0.05 and 0, scaled by 0.995. */
    Sample decayedTerm = 0.995 * (-0.05 + 0) / 2.0;
    EXPECT_EQUAL(instrument.waveform[1], decayedTerm);

    /* Get two more samples. The waveform is [0, decayedTerm], so we should
     * get back 0, then decayedTerm.
     */
    EXPECT_EQUAL(instrument.nextSample(), 0.0);
    EXPECT_EQUAL(instrument.cursor, 1);
    EXPECT_EQUAL(instrument.nextSample(), decayedTerm);
    EXPECT_EQUAL(instrument.cursor, 0);

    /* The first array value is the average of 0.0 and decayedTerm, scaled by
     * 0.995.
     */
    Sample moreDecayed = 0.995 * (decayedTerm + 0) / 2.0;
    EXPECT_EQUAL(instrument.waveform[0], moreDecayed);

    /* The second array value is the average of decayedTerm and moreDecayed,
     * scaled by 0.995.
     */
    EXPECT_EQUAL(instrument.waveform[1], 0.995 * (decayedTerm + moreDecayed) / 2.0);
}

/* * * * * Special Functions Below This Point * * * * */

#include <algorithm>

/* Copy constructor for StringInstrument. This is called automatically by C++
 * if you need to make a copy of a StringInstrument; say, if you were to pass
 * one into a function by value. Take ProjectL for more details about how this
 * works.
 */
StringInstrument::StringInstrument(const StringInstrument& rhs) {
    /* Not normally part of a copy constructor, this line is designed
     * to ensure that if you haven't implemented StringInstrument,
     * you don't get weird and wild crashes.
     */
    if (rhs.waveform == nullptr) {
        return;
    }

    length   = rhs.length;
    waveform = new Sample[length];
    cursor   = rhs.cursor;

    for (int i = 0; i < length; i++) {
        waveform[i] = rhs.waveform[i];
    }
}

/* project operator for StringInstrument. This is called automatically by
 * C++ when you assign one StringInstrument to another. Take ProjectL for more
 * details about how this works. This specific implementation uses an idiom
 * called "copy-and-swap."
 */
StringInstrument& StringInstrument::operator =(StringInstrument rhs) {
    swap(length,   rhs.length);
    swap(waveform, rhs.waveform);
    swap(cursor,   rhs.cursor);
    return *this;
}
