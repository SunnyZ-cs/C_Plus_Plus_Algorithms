#include "ShiftScheduling.h"
using namespace std;

/* This helper function checks if a shift overlaps with a shift that the employee has already
 * been assigned.
 */
bool isOverlapping(const Shift& curShift, const Set<Shift>& soFar) {
    for (Shift shift: soFar) {
        if (overlapsWith(curShift, shift)) {
            return true;
        }
    }
    return false;
}

/* Main implementation here*/
int numWaysFor(const Set<Shift>& shiftsLeft,
               const Set<Shift>& soFar,
               const int& maxHours) {
    /* Base case 1: if the maximum hours available for project is 0, there is only one way to assign
     * shifts to the employee, which is "not assigning any shifts at all".
     */
    if (maxHours == 0) {
        return 1;
    }

    /* Base case 2: if the set of shifts left is empty, there is only one way to assign shifts to
     * the employee, which is "not assigning any shifts at all".
     */
    if (shiftsLeft.isEmpty()) {
        return 1;
    }

    /* Recursive case: select some shift out of the set of shifts left, then look at two possible branches
     * separately.
     */
    Shift curShift = shiftsLeft.first();
    int without = numWaysFor(shiftsLeft - curShift, soFar, maxHours);

    /* Employee can't make the shift because it overlaps with an existing shift or it exceeds the current
     * maximum number of hours possible for assigning shifts. Do not give employee that shift.
     */
    if (isOverlapping(curShift, soFar) || lengthOf(curShift) > maxHours) {
        return without;
    } else {
        /* Shift passed the constraints test. Employee can theoretically make the shift. So, either give
         * employee the shift or don't give employee the shift.
         */
        int with = numWaysFor(shiftsLeft - curShift, soFar + curShift, maxHours - lengthOf(curShift));
        return with + without;
    }
}

/* Wrapper function */
int numSchedulesFor(const Set<Shift>& shifts, int maxHours) {
    /* Error case elimination */
    if (maxHours < 0) {
        error("Maximum hours available is negative.");
    }

    return numWaysFor(shifts, {}, maxHours);
}

/* This helper function helps to compute the total profit of all shifts in a specific
 * set of shifts.
 */
int totalProfitOf(const Set<Shift>& shifts) {
    int total = 0;
    for (Shift shift: shifts) {
        total += profitFor(shift);
    }
    return total;
}

/* Main implementation here.*/
Set<Shift> maxProfitRec(const Set<Shift>& shiftsLeft, int maxHours, const Set<Shift>& soFar) {
    /* Base case 1:
     * if the maximum hours left is 0, then no further shifts can be added to the existing set of assigned
     * shifts (i.e. the soFar set is final.) If the total profit of the shifts schedule we've decided so far
     * is negative, then it's better to just assign nothing (in which case profit is 0 but at least not making
     * a loss).
     */
    if (maxHours == 0) {
        if (totalProfitOf(soFar) < 0) {
            return {};
        }
        return soFar;
    }

    /* Base case 2:
     * if the set of shifts left is empty, then no further shifts can be added to the existing set of assigned
     * shifts. By a similar reasoning as in base case 1, we do something similar below.
     */
    if (shiftsLeft.isEmpty()) {
        if (totalProfitOf(soFar) < 0) {
            return {};
        }
        return soFar;
    }

    /* Recursive case:
     * Select a shift out of the set of shifts left. Determine if it's possible to be added to the existing set of
     * assgined shifts. If not, we can only make our optimal schedule from all shifts left but without the current
     * shift; if it can indeed be added, we then look at the two optimization branches (with/without the shift we
     * just selected) and optimize again (i.e. return the branch that yields a higher profit.)
     */
    Shift curShift = shiftsLeft.first();
    Set<Shift> maxProfitWithout = maxProfitRec(shiftsLeft - curShift, maxHours, soFar);

    if (isOverlapping(curShift, soFar) || lengthOf(curShift) > maxHours) {
        return maxProfitWithout;
    } else {
        Set<Shift> maxProfitWith = maxProfitRec(shiftsLeft - curShift, maxHours - lengthOf(curShift), soFar + curShift);
        if (totalProfitOf(maxProfitWith) > totalProfitOf(maxProfitWithout)) {
            return maxProfitWith;
        } else {
            return maxProfitWithout;
        }
    }
}

/* Wrapper function */
Set<Shift> maxProfitSchedule(const Set<Shift>& shifts, int maxHours) {
    /* Error case elimination */
    if (maxHours < 0) {
        error("Maximum hours available is negative.");
    }

    return maxProfitRec(shifts, maxHours, {});
}




/* * * * * * Unit Tests Curated and Written by Sunny Zhao * * * * * */
#include "GUI/SimpleTest.h"

STUDENT_TEST("numSchedulesFor handles edge cases with empty shifts or 0 hour limit.") {
    /* If the set of available shifts for project is empty, there is only one way to
     * assign shifts.
     */
    EXPECT_EQUAL(numSchedulesFor({}, 10), 1);

    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17 },  // Monday, 9AM - 5PM
    };

    /* If the maximum hour limit is 0, there is only one way to assign shifts.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 0), 1);
}

STUDENT_TEST("maxProfitSchedule handles shifts with negative profits(so making a loss).") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17, -100 },  // Monday, 9AM - 5PM, value is -100
    };

    /* Even with exactly the right amount of time, you should NOT pick the one shift because
     * making a loss is even worse than making 0 profit.
     */
    EXPECT_EQUAL(maxProfitSchedule(shifts, 24), {});
}




/* * * * * * Test cases from the starter files below this point. * * * * * */
#include "vector.h"
#include "error.h"


STUDENT_TEST("numSchedulesFor reports errors with illegal numbers of hours.") {
    EXPECT_ERROR(numSchedulesFor({}, -137));
    EXPECT_ERROR(numSchedulesFor({}, -1));

    /* However, it's fine to ask for zero hours. */
    EXPECT_NO_ERROR(numSchedulesFor({}, 0));
}

STUDENT_TEST("numSchedulesFor works with just one shift.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17 },  // Monday, 9AM - 5PM
    };

    /* With unbounded time, you have two options: either don't assign the
     * worker anything, or assign the worker the one shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 24), 2);

    /* With exactly the right amount of time, you still have two options. */
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 2);

    /* With just too little time, you only have on option, which is to not
     * pick the shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 7), 1);
}

STUDENT_TEST("numSchedulesFor does not pick overlapping shifts.") {
    Set<Shift> shifts = {
        { Day::MONDAY,  9, 17 },  // Monday,  9AM - 5PM
        { Day::MONDAY, 10, 18 },  // Monday, 10AM - 6PM
    };

    /* With unbounded time, there are three choices: nothing, or the
     * first shift, or the second shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 100), 3);

    /* With exactly eight hours, you still have the same three options. */
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 3);

    /* With fewer than eight hours, there is only one option, which is
     * to not give the worker anything.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 7), 1);
}

STUDENT_TEST("numSchedulesFor does not pick shifts exceeding time limit.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 20 }, // 10-hour shift
        { Day::TUESDAY,   10, 15 }, //  5-hour shift
        { Day::WEDNESDAY, 10, 16 }, //  6-hour shift
    };

    /* With unbounded time, you can pick any of the eight subsets of these
     * shifts.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 8);

    /* With eleven hours, you can pick
     *   nothing,
     *   the 10-hour shift,
     *   the 5-hour shift,
     *   the 6-hour shift, or
     *   the 5- and 6-hour shifts.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 11), 5);

    /* With ten hours, you can pick
     *   nothing,
     *   the 10-hour shift,
     *   the 5-hour shift, or
     *   the 6-hour shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 10), 4);

    /* With six hours, you can pick
     *   nothing,
     *   the 5-hour shift, or
     *   the 6-hour shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 6), 3);
}

STUDENT_TEST("numSchedulesFor handles one pair of overlapping shifts.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 14 }, //  4-hour shift
        { Day::MONDAY,    12, 16 }, //  4-hour shift
        { Day::WEDNESDAY, 10, 14 }, //  4-hour shift
    };

    /* With unbounded time, you can pick either
     *    nothing,
     *    the first Monday shift,
     *    the second Monday shift,
     *    the Wednesday shift,
     *    the first Monday shift and the Wednesday shift, or
     *    the second Monday shift and the Wednesday shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 6);

    /* With eight hours, all those options are still available. */
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 6);
}

STUDENT_TEST("numSchedulesFor handles two pairs of overlapping shifts.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 14 }, //  4-hour shift
        { Day::MONDAY,    12, 16 }, //  4-hour shift
        { Day::WEDNESDAY, 10, 14 }, //  4-hour shift
        { Day::WEDNESDAY, 12, 16 }, //  4-hour shift
    };

    /* With unbounded time, you can pick either
     *    nothing,
     *    the first Monday shift,
     *    the second Monday shift,
     *    the first Wednesday shift,
     *    the second Wednesday shift,
     *    the first Monday shift and the first Wednesday shift,
     *    the first Monday shift and the second Wednesday shift,
     *    the second Monday shift and the first Wednesday shift, or
     *    the second Monday shift and the second Wednesday shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 9);

    /* With eight hours, all those options are still available. */
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 9);
}

STUDENT_TEST("numSchedulesFor handles two pairs of overlapping shifts with time limits.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 14 }, //  4-hour shift
        { Day::MONDAY,    12, 18 }, //  6-hour shift
        { Day::WEDNESDAY, 10, 14 }, //  4-hour shift
        { Day::WEDNESDAY, 12, 18 }, //  6-hour shift
    };

    /* With unbounded time, you can pick either
     *    nothing,
     *    the first Monday shift,
     *    the second Monday shift,
     *    the first Wednesday shift,
     *    the second Wednesday shift,
     *    the first Monday shift and the first Wednesday shift,
     *    the first Monday shift and the second Wednesday shift,
     *    the second Monday shift and the first Wednesday shift, or
     *    the second Monday shift and the second Wednesday shift.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 9);

    /* With eight hours, you cannot pick the second Monday or second
     * Wednesday shift with anything else.
     */
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 6);
}

STUDENT_TEST("numSchedulesFor passes the example from the project description.") {
    Set<Shift> shifts = {
        { Day::MONDAY,     8, 12 },  // Mon  8AM - 12PM
        { Day::MONDAY,    12, 16 },  // Mon 12PM -  4PM
        { Day::MONDAY,    16, 20 },  // Mon  4PM -  8PM
        { Day::MONDAY,     8, 14 },  // Mon  8AM -  2PM
        { Day::MONDAY,    14, 20 },  // Mon  2PM -  8PM
        { Day::TUESDAY,    8, 12 },  // Tue  8AM - 12PM
        { Day::TUESDAY,   12, 16 },  // Tue 12PM -  4PM
        { Day::TUESDAY,   16, 20 },  // Tue  4PM -  8PM
        { Day::TUESDAY,    8, 14 },  // Tue  8AM -  2PM
        { Day::TUESDAY,   14, 20 },  // Tue  2PM -  8PM
        { Day::WEDNESDAY,  8, 12 },  // Wed  8AM - 12PM
        { Day::WEDNESDAY, 12, 16 },  // Wed 12PM -  4PM
        { Day::WEDNESDAY, 16, 20 },  // Wed  4PM -  8PM
        { Day::WEDNESDAY,  8, 14 },  // Wed  8AM -  2PM
        { Day::WEDNESDAY, 14, 20 },  // Wed  2PM -  8PM
    };

    /* There should be 1,044 options with 20 hours available. */
    EXPECT_EQUAL(numSchedulesFor(shifts, 20), 1044);
}

STUDENT_TEST("numSchedulesFor stress test: Don't generate combinations with overlapping shifts.") {
    /* All of these shifts overlap one another. If you try producing all combinations
     * of these shifts and only check at the end whether they're valid, you'll be
     * checking 2^100 ~= 10^30 combinations of shifts, which will take so long the
     * sun will have burnt out before you're finished.
     * Instead, as you're going through your decision tree and building up your shifts,
     * make sure not to include any shifts that clearly conflict with something you
     * picked earlier.
     */
    Set<Shift> trickySet;
    for (int i = 0; i < 100; i++) {
        trickySet += Shift{ Day::MONDAY, i, i + 200 };
    }
    EXPECT_EQUAL(trickySet.size(), 100);

    /* You can pick at most one of the shifts. There are 100 shifts, plus
     * the one remaining option of picking nothing at all.
     */
    EXPECT_EQUAL(numSchedulesFor(trickySet, 300), 101);
}

STUDENT_TEST("numSchedulesFor stress test: Don't generate options exceeding time limit.") {
    /* Here's a collection of one shift per hour of the week. Your worker has exactly
     * one hour free. If you try all possible combinations of these shifts, ignoring time
     * constraints, you will have to check over 2^100 = 10^30 combinations, which will
     * take longer than the length of the known universe to process.
     * Instead, as you're exploring the decision tree to generate shift combinations,
     * make sure not to add shifts that would exceed the time limit.
     */
    Set<Shift> trickySet;
    for (Day day: { Day::SUNDAY,
                    Day::MONDAY,
                    Day::TUESDAY,
                    Day::WEDNESDAY,
                    Day::THURSDAY,
                    Day::FRIDAY,
                    Day::SATURDAY}) {
        for (int start = 0; start < 24; start++) {
            trickySet += Shift{ day, start, start + 1, 10 };
        }
    }
    EXPECT_EQUAL(trickySet.size(), 7 * 24);

    /* There are 7 * 24 different shifts you can pick, and you can only pick one
     * of them. Add in the option of picking nothing and you have all the options.
     */
    EXPECT_EQUAL(numSchedulesFor(trickySet, 1), 1 + 7 * 24);
}

STUDENT_TEST("numSchedulesFor stress test: Handles realistic set of shifts") {
    /* Available shifts. */
    Set<Shift> shifts = {
        { Day::SUNDAY,  8, 14 },
        { Day::SUNDAY, 12, 18 },

        { Day::MONDAY,  8, 12 },
        { Day::MONDAY, 12, 16 },
        { Day::MONDAY, 16, 20 },
        { Day::MONDAY,  8, 16 },
        { Day::MONDAY, 12, 20 },

        { Day::TUESDAY,  8, 12 },
        { Day::TUESDAY, 12, 16 },
        { Day::TUESDAY, 16, 20 },
        { Day::TUESDAY,  8, 16 },
        { Day::TUESDAY, 12, 20 },

        { Day::WEDNESDAY,  8, 12 },
        { Day::WEDNESDAY, 12, 16 },
        { Day::WEDNESDAY, 16, 20 },
        { Day::WEDNESDAY,  8, 16 },
        { Day::WEDNESDAY, 12, 20 },

        { Day::THURSDAY,  8, 12 },
        { Day::THURSDAY, 12, 16 },
        { Day::THURSDAY, 16, 20 },
        { Day::THURSDAY,  8, 16 },
        { Day::THURSDAY, 12, 20 },

        { Day::FRIDAY,  8, 12 },
        { Day::FRIDAY, 12, 16 },
        { Day::FRIDAY, 16, 20 },
        { Day::FRIDAY,  8, 16 },
        { Day::FRIDAY, 12, 20 },

        { Day::SATURDAY,  8, 14 },
        { Day::SATURDAY, 12, 18 },
    };

    /* There are 38,107 possible schedules you can form from these possible
     * shifts, assuming you have at most 25 hours available.
     * Your implementation should be able to solve this instance in at most
     * three seconds. Where did we get three seconds from? We ran our
     * reference implementation on a middle-of-the-line computer and added
     * a 10x safety factor.
     */
    EXPECT_COMPLETES_IN(3.0, {
        EXPECT_EQUAL(numSchedulesFor(shifts, 25), 38107);
    });
}












/* This nice utility function lets you call highestValueScheduleFor, passing in
 * a Vector of shifts rather than a Set. This makes it a bit easier to test things.
 * You shouldn't need this function outside of these test cases.
 */
Set<Shift> asSet(const Vector<Shift>& shifts) {
    Set<Shift> result;
    for (Shift s: shifts) {
        result += s;
    }
    return result;
}

STUDENT_TEST("maxProfitSchedule works on one shift.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday, 9AM - 5PM, value is 1000
    };

    /* With unbounded time, you should pick the one shift. */
    EXPECT_EQUAL(maxProfitSchedule(shifts, 24), shifts);

    /* With exactly the right amount of time, you should pick the one shift. */
    EXPECT_EQUAL(maxProfitSchedule(shifts, 24), shifts);

    /* With too little time, you cannot pick the shift. */
    EXPECT_EQUAL(maxProfitSchedule(shifts, 1), {});
}

STUDENT_TEST("maxProfitSchedule works on a pair of overlapping shifts.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,  9, 17, 1000 },  // Monday,  9AM - 5PM, value is 1000
        { Day::MONDAY, 10, 18, 2000 },  // Monday, 10AM - 6PM, value is 2000
    };

    /* Pick the second one; it's better. */
    EXPECT_EQUAL(maxProfitSchedule(asSet(shifts), 100), { shifts[1] });
}

STUDENT_TEST("maxProfitSchedule doesn't always use highest-value shift.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,    10, 20, 1000 }, // 10-hour shift, value is 1000
        { Day::TUESDAY,   10, 15,  500 }, //  5-hour shift, value is 500
        { Day::WEDNESDAY, 10, 16,  501 }, //  6-hour shift, value is 501
    };

    /* The correct strategy is to forgo the highest-value shift in favor of the two
     * shorter shifts.
     */
    auto schedule = maxProfitSchedule(asSet(shifts), 11);
    EXPECT_EQUAL(schedule, { shifts[1], shifts[2] });
}

STUDENT_TEST("maxProfitSchedule doesn't always use shift with highest value per time.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,    10, 17, 21 }, //  7-hour shift, value is 21 ($3 / hour)
        { Day::TUESDAY,   10, 16, 12 }, //  6-hour shift, value is 12 ($2 / hour)
        { Day::WEDNESDAY, 10, 16, 12 }, //  6-hour shift, value is 12 ($2 / hour)
    };

    /* If you have 12 hours, the correct strategy is to pick the two six-hour shifts
     * for a total of $24 value. Picking the shift with the highest value per unit
     * time (the seven-hour shift) produces only $21 value.
     */
    auto schedule = maxProfitSchedule(asSet(shifts), 12);
    EXPECT_EQUAL(schedule, { shifts[1], shifts[2] });
}

STUDENT_TEST("maxProfitSchedule works on the example from the project description.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27 *
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28 *
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25 *
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25 *
    };

    /* Get back the solution. */
    Set<Shift> computedSolution = maxProfitSchedule(asSet(shifts), 20);

    /* Form the correct answer. It's the starred entries. */
    Set<Shift> actualSolution = {
        shifts[0], shifts[1], shifts[2], shifts[14]
    };

    EXPECT_EQUAL(computedSolution, actualSolution);
}

STUDENT_TEST("maxProfitSchedule reports an error with negative hours.") {
    /* From the project description. */
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25
    };

    /* Should be an error. */
    EXPECT_ERROR(maxProfitSchedule(asSet(shifts), -1));

    /* Still an error even if there are no shifts. */
    EXPECT_ERROR(maxProfitSchedule({}, -1));
}

STUDENT_TEST("maxProfitSchedule handles zero free hours.") {
    /* From the project description. */
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25
    };

    /* Shouldn't be an error if time is zero - that means we just don't pick anything. */
    EXPECT_EQUAL(maxProfitSchedule(asSet(shifts), 0).size(), 0);
}

STUDENT_TEST("maxProfitSchedule stress test: Don't generate combinations with overlaps.") {
    /* All of these shifts overlap one another. If you try producing all combinations
     * of these shifts and only check at the end whether they're valid, you'll be
     * checking 2^100 ~= 10^30 combinations of shifts, which will take so long the
     * sun will have burnt out before you're finished.
     * Instead, as you're going through your decision tree and building up your shifts,
     * make sure not to include any shifts that clearly conflict with something you
     * picked earlier.
     */
    Set<Shift> trickySet;
    for (int i = 0; i < 100; i++) {
        trickySet += Shift{ Day::MONDAY, 1, 2, i };
    }
    EXPECT_EQUAL(trickySet.size(), 100);

    auto result = maxProfitSchedule(trickySet, 1);
    EXPECT_EQUAL(result.size(), 1);
}

STUDENT_TEST("maxProfitSchedule stress test: Don't generate combinations exceeding time limit.") {
    /* Here's a collection of one shift per hour of the week. Your worker has exactly
     * one hour free. If you try all possible combinations of these shifts, ignoring time
     * constraints, you will have to check over 2^100 = 10^30 combinations, which will
     * take longer than the length of the known universe to process.
     * Instead, as you're exploring the decision tree to generate shift combinations,
     * make sure not to add shifts that would exceed the time limit.
     */
    Set<Shift> trickySet;
    for (Day day: { Day::SUNDAY,
                    Day::MONDAY,
                    Day::TUESDAY,
                    Day::WEDNESDAY,
                    Day::THURSDAY,
                    Day::FRIDAY,
                    Day::SATURDAY}) {
        for (int start = 0; start < 24; start++) {
            trickySet += Shift{ day, start, start + 1, 10 };
        }
    }
    EXPECT_EQUAL(trickySet.size(), 7 * 24);

    auto result = maxProfitSchedule(trickySet, 1);
    EXPECT_EQUAL(result.size(), 1);
}

STUDENT_TEST("maxProfitSchedule stress test: Handles realistic example.") {
    /* Available shifts. */
    Vector<Shift> shifts = {
        { Day::SUNDAY,  8, 14, 12 },
        { Day::SUNDAY, 12, 18, 36 },

        { Day::MONDAY,  8, 12, 44 },
        { Day::MONDAY, 12, 16, 32 },
        { Day::MONDAY, 16, 20,  0 },
        { Day::MONDAY,  8, 16, 16 },
        { Day::MONDAY, 12, 20, 22 },

        { Day::TUESDAY,  8, 12, 48 },
        { Day::TUESDAY, 12, 16, 20 },
        { Day::TUESDAY, 16, 20, 24 },
        { Day::TUESDAY,  8, 16, 24 },
        { Day::TUESDAY, 12, 20, 80 },

        { Day::WEDNESDAY,  8, 12, 20 },
        { Day::WEDNESDAY, 12, 16,  8 },
        { Day::WEDNESDAY, 16, 20,  8 },
        { Day::WEDNESDAY,  8, 16, 40 },
        { Day::WEDNESDAY, 12, 20, 16 },

        { Day::THURSDAY,  8, 12, 40 },
        { Day::THURSDAY, 12, 16,  0 },
        { Day::THURSDAY, 16, 20, 24 },
        { Day::THURSDAY,  8, 16, 56 },
        { Day::THURSDAY, 12, 20, 32 },

        { Day::FRIDAY,  8, 12,  4 },
        { Day::FRIDAY, 12, 16,  8 },
        { Day::FRIDAY, 16, 20, 40 },
        { Day::FRIDAY,  8, 16, 72 },
        { Day::FRIDAY, 12, 20, 40 },

        { Day::SATURDAY,  8, 14, 18 },
        { Day::SATURDAY, 12, 18, 66 },
    };

    /* This code should finish in at most four seconds. That is a 10x margin of safety
     * over our unoptimized reference implementation run on a middle-of-the-line
     * computer.
     */
    EXPECT_COMPLETES_IN(4.0, {
        auto answer = maxProfitSchedule(asSet(shifts), 25);
        EXPECT_EQUAL(answer, { shifts[2], shifts[7], shifts[11], shifts[17], shifts[24] });
    });
}
