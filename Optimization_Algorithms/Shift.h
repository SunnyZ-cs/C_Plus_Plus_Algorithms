#pragma once

#include <string>
#include <ostream>

/* Type representing a day of the week. */
enum class Day {
    SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY
};

/* Type representing a weekday shift. As a simplifying assumption, we're assuming that
 * every shift occurs only a particular calendar day, so you can't, say, have a shift
 * from 10:00PM on Monday to 2:00AM on Tuesday. This is just to make your life a bit
 * easier in the process of coding this up - it's not meant to suggest that all shifts
 * are nicely aligned this way IRL.
 */
class Shift {
public:
    Shift() = default;
    Shift(Day day, int startHour, int endHour, int value = 0);

    /* Accessors to get individual fields. You shouldn't need to use this
     * for your implementation; it's here for the graphics system to be
     * able to display things correctly.
     */
    Day day() const {
        return day_;
    }
    int startHour() const {
        return startHour_;
    }
    int endHour() const {
        return endHour_;
    }

    friend int profitFor(const Shift& shift);

private:
    Day day_ = Day::SUNDAY;
    int startHour_ = 0; // Inclusive.
    int endHour_ = 0;   // Exclusive. Could be 24 to mean "up through midnight"
    int value_ = 0;     // How much value this shift produced.
};

/* Given two shifts, reports whether those shifts overlap with one another. */
bool overlapsWith(const Shift& one, const Shift& two);

/* Given a shift, returns its length or value. These are provided purely for convenience. */
int lengthOf(const Shift& shift);

/* These operators make it possible to store Shifts in Maps and HashMaps, if that's
 * something you'd like to do.
 */
bool operator<  (const Shift& lhs, const Shift& rhs);
bool operator== (const Shift& lhs, const Shift& rhs);
int  hashCode   (const Shift& shift);

/* These operator makes it possible to print out Shifts, if that's
 * something you'd like to do.
 */
std::ostream& operator<< (std::ostream& out, const Shift& shift);

/* Function to print out a Day. */
std::ostream& operator<< (std::ostream& out, Day day);
