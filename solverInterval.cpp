#include "solverInterval.hpp"
#include <algorithm>
#include <numeric>

void SolverInterval::computeSolutionFromIntervals() {
    uint compt = 1;
    for (auto it = solutionInterval.begin(); it != solutionInterval.end(); ++it) {
        for (uint i = it->first; i <= it->second; i++) {
            solution.at(i) = compt;
        }
        compt++;
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
    if (checkIsSorted()) return;

    std::vector<size_t> indices(N);
    std::iota(indices.begin(), indices.end(), 0);

    std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
        return getCoordinate(a, 0) < getCoordinate(b, 0);
    });

    std::vector<double> sortedPoints(N * D);
    for (size_t i = 0; i < N; ++i) {
        for (size_t d = 0; d < D; ++d) {
            sortedPoints[i * D + d] = getCoordinate(indices[i], d);
        }
    }

    points = std::move(sortedPoints);
    isSorted = true;
}