#pragma once

#include "solver.hpp"
#include <utility>


class SolverInterval : public Solver {

protected:

    vector<pair<uint,uint>> solutionInterval; // ex : "solutionInterval = { {0, 2}, {3, 5}, {6, 7} };" '{0, 2}' will belong to cluster N1 ...

    /**
     * @brief assign clusters to intervals based on solutionInterval
     */
    void computeSolutionFromIntervals();


    bool checkIsSorted();

    /**
     * @brief Sort points according to first dimension (useful for DP approaches)
     */
    void resort();
};



