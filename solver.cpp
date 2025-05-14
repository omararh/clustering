#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <stdexcept>

#include "solver.hpp"

// Private method implementation
void Solver::displayPoint(size_t index) const {
    std::cout << "( ";
    for (size_t d = 0; d < D; ++d) {
        std::cout << getCoordinate(index, d);
        if (d < D - 1) std::cout << ", ";
    }
    std::cout << " )";
}

// Protected method implementations
double Solver::squaredDistance(size_t i, size_t j) const {
    double result = 0.0;
    for (size_t d = 0; d < D; ++d) {
        double diff = getCoordinate(i, d) - getCoordinate(j, d);
        result += diff * diff;
    }
    return result;
}

double Solver::distance(size_t i, size_t j) const {
    return std::sqrt(squaredDistance(i, j));
}

double Solver::distancePower(size_t i, size_t j, double power) const {
    return std::pow(squaredDistance(i, j), 0.5 * power);
}

bool Solver::verifyParetoFront() const {
    if (N <= 1) return true;  // Empty or single point is trivially a Pareto front

    // For each point, check that it's not dominated by any other point
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (i == j) continue;

            // Check if point j dominates point i
            bool isDominated = true;
            bool isStrictlyBetter = false;

            for (size_t d = 0; d < D; ++d) {
                double val_i = getCoordinate(i, d);
                double val_j = getCoordinate(j, d);

                // For a point to dominate, it must be <= in all dimensions
                if (val_j > val_i) {
                    isDominated = false;
                    break;
                }

                // And strictly better in at least one dimension
                if (val_j < val_i) {
                    isStrictlyBetter = true;
                }
            }

            if (isDominated && isStrictlyBetter) {
                return false;  // Found a dominated point, not a valid Pareto front
            }
        }
    }

    return true;  // No dominated points found
}

void Solver::sortPointsByFirstDimension() {
    if (isSorted) return;

    // Create a vector of indices
    std::vector<size_t> indices(N);
    for (size_t i = 0; i < N; ++i) {
        indices[i] = i;
    }

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

// Public method implementations
void Solver::import(const std::string& filename) {
    points.clear();
    solution.clear();
    isSorted = false;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Fatal error: unable to read file " << filename << "!" << std::endl;
        throw std::runtime_error("File not found: " + filename);
    }

    // Read number of points
    double tmpValue;
    file >> tmpValue;
    N = static_cast<size_t>(tmpValue);

    if (!file.good()) {
        std::cerr << "Fatal error: cannot read number of points!" << std::endl;
        throw std::runtime_error("Invalid file format");
    }

    // Read dimension
    file >> tmpValue;
    D = static_cast<size_t>(tmpValue);

    if (!file.good()) {
        std::cerr << "Fatal error: cannot read dimension!" << std::endl;
        throw std::runtime_error("Invalid file format");
    }

    std::cout << "Number of points: " << N << std::endl;
    std::cout << "Dimension: " << D << std::endl;

    // Reserve space for points
    points.reserve(N * D);

    // Read all points
    size_t count = 0;
    while (file >> tmpValue && count < N * D) {
        points.push_back(tmpValue);
        count++;
    }

    if (count != N * D) {
        std::cerr << "Fatal error: expected " << (N * D) << " coordinates, but read " << count << std::endl;
        throw std::runtime_error("Insufficient data in file");
    }

    // Initialize solution vector
    solution.resize(N, 0);  // No cluster assigned initially
}

void Solver::displayInstance() const {
    std::cout << "Instance with " << N << " points of dimension " << D << ":" << std::endl;
    for (size_t i = 0; i < N; ++i) {
        std::cout << "Point " << i << ": ";
        displayPoint(i);
        std::cout << std::endl;
    }
}

void Solver::displaySolution() const {
    std::cout << "\nClustering solution with " << K << " clusters (total cost: " << solutionCost << "):" << std::endl;

    // Count points in each cluster
    std::vector<size_t> clusterSizes(K+1, 0);  // +1 because clusters are 1-indexed
    for (size_t i = 0; i < N; ++i) {
        clusterSizes[solution[i]]++;
    }

    // Display each cluster
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

void Solver::exportSolution(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << " for writing!" << std::endl;
        return;
    }

    file << "# Clustering solution" << std::endl;
    file << "# Number of points: " << N << std::endl;
    file << "# Dimension: " << D << std::endl;
    file << "# Number of clusters: " << K << std::endl;
    file << "# Total cost: " << solutionCost << std::endl;
    file << "#" << std::endl;

    file << "# Format: point_index cluster_index" << std::endl;
    for (size_t i = 0; i < N; ++i) {
        file << i << " " << solution[i] << std::endl;
    }

    file.close();
    std::cout << "Solution exported to " << filename << std::endl;
}

// Solver2D implementation
double Solver2D::squaredDistance(size_t i, size_t j) const {
    // Optimized implementation for 2D
    double dx = getCoordinate(i, 0) - getCoordinate(j, 0);
    double dy = getCoordinate(i, 1) - getCoordinate(j, 1);
    return dx * dx + dy * dy;
}