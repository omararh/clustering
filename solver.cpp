#include "solver.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

void Solver::import(const std::string& filename) {
    points.clear();
    solution.clear();
    isSorted = false;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("File not found: " + filename);
    }

    double tmpValue;
    file >> tmpValue;
    N = static_cast<size_t>(tmpValue);

    file >> tmpValue;
    D = static_cast<size_t>(tmpValue);

    std::cout << "Number of points: " << N << std::endl;
    std::cout << "Dimension: " << D << std::endl;

    points.reserve(N * D);

    size_t count = 0;
    while (file >> tmpValue && count < N * D) {
        points.push_back(tmpValue);
        count++;
    }

    if (count != N * D) {
        throw std::runtime_error("Insufficient data in file");
    }

    solution.resize(N, 0);
}

void Solver::displaySolution() const {
    std::cout << "\nClustering solution with " << K << " clusters (total cost: " << solutionCost << "):" << std::endl;

    std::vector<size_t> clusterSizes(K+1, 0);
    for (size_t i = 0; i < N; ++i) {
        clusterSizes[solution[i]]++;
    }

    for (size_t k = 1; k <= K; ++k) {
        std::cout << "Cluster " << k << " (" << clusterSizes[k] << " points): ";
        bool first = true;
        for (size_t i = 0; i < N; ++i) {
            if (solution[i] == k) {
                if (!first) std::cout << ", ";
                displayPoint(i);
                first = false;
            }
        }
        std::cout << std::endl;
    }
}