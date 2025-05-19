#include <algorithm>
#include <iostream>
#include <numeric>
#include "solverInterval.hpp"


void SolverInterval::computeSolutionFromIntervals(){
    uint compt=1;
    for (vector<pair<uint, uint>>::iterator it2 = solutionInterval.begin() ; it2 != solutionInterval.end(); ++it2){
        for (uint i = it2->first; i<= it2->second ; i++)
            solution.at(i) = compt;
        compt ++;
    }
}

bool SolverInterval::checkIsSorted() {
    for (size_t i = 1; i < N; ++i) {
        if (getCoordinate(i - 1, 0) > getCoordinate(i, 0)) {
            isSorted = false;
            return false;
        }
    }
    isSorted = true;
    return true;
}

void SolverInterval::resort() {
    if (isSorted) return;

    // Create a vector of indices
    std::vector<size_t> indices(N);
    std::iota(indices.begin(), indices.end(), 0); // fill from 0 to N-1

    // Sort indices based on the first dimension
    std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
        return getCoordinate(a, 0) > getCoordinate(b, 0);  // Descending order
    });

    // Create a new points vector with the sorted order
    std::vector<double> sortedPoints(N * D);
    for (size_t i = 0; i < N; ++i) {
        for (size_t d = 0; d < D; ++d) {
            sortedPoints[i * D + d] = getCoordinate(indices[i], d);
        }
    }

    // Replace the original points with the sorted ones
    points = std::move(sortedPoints);
    isSorted = true;
}