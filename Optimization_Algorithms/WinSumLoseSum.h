#pragma once

#include "Demos/optional.h"
#include "set.h"

/* Given a set of numbers and a target, returns one way to select a subset of those
 * numbers that add up to the target if one exists.
 */
Optional<Set<int>> makeTarget(const Set<int>& elems, int target);
