#include "FirstBranchingStrategy.h"

#include <algorithm>
#include <vector>
#include "boolequation.h"

int FirstBranchingStrategy::ChooseColForBranching(BoolEquation& equation) {
    for (int i = 0; i < equation.mask.getSize(); i++) {
        if (equation.mask[i] == 0) {
            return i;
        }
    }

    return -1;
}
