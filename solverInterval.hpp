#pragma once
#include "solver.hpp"
#include <utility>

typedef unsigned int uint;

class SolverInterval : public Solver {
protected:
    vector<pair<uint, uint>> solutionInterval;

    void computeSolutionFromIntervals();
    bool checkIsSorted();
    void resort();
};