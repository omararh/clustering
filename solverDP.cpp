#include <iostream>
#include <limits>
#include <vector>
#include "solverDP.hpp"

using namespace std;

/**
 * Display the final costs of each cluster configuration
 *
 * @param sep Separator to use between values
 */
void SolverDP::printFinalCosts(string sep) {
    cout << endl;

    if (!isMatrixAvailable()) {
        return;
    }

    for (uint i = 0; i < K; i++) {
        if (i < matrixDP.getRows() && N-1 < matrixDP.getCols()) {
            cout << "avec " << i + 1 << " clusters = "<< matrixDP.getElement(i, N-1) << sep;
        } else {
            cout << "NA" << sep;
        }
    }
    cout << endl;
}

/**
 * Print the dynamic programming matrix
 */
void SolverDP::printMatrixDP() {
    cout << endl;

    if (!isMatrixAvailable()) {
        return;
    }

    for (int i = K-1; i >= 0; i--) {
        for (uint n = 0; n < N; n++) {
            if (i < static_cast<int>(matrixDP.getRows()) && n < matrixDP.getCols()) {
                cout << matrixDP.getElement(i, n) << " ";
            } else {
                cout << "NA ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

/**
 * Fill the first row of the DP matrix with costs for single-cluster configurations
 */
void SolverDP::fillFirstLine(vector<double>& v) {
    if (N == 0) return;

    // Calculate costs for clusters that start from the beginning of the array
    clusterCostsFromBeginning(v);

    // Fill the first row of the matrix with these costs
    for (uint i = 0; i < N && i < matrixDP.getCols(); i++) {
        matrixDP.setElement(0, i, v[i]);
    }
}

/**
 * Main solve method implementing the dynamic programming algorithm
 */
void SolverDP::solve() {
    // Input validation
    if (!validateInputs()) {
        return;
    }

    // Ensure points are sorted on the first dimension
    resort();

    // Initialize DP matrix
    initializeMatrix();

    vector<double> v(N, 0.0);
    // Fill the first line representing single cluster costs
    fillFirstLine(v);

    // Fill the rest of the matrix using dynamic programming
    fillDPMatrix(v);

    // Backtracking to build the solution
    buildSolutionFromMatrix();

    // Compute final solution from intervals
    computeSolutionFromIntervals();

    // Calculate the final cost
    calculateFinalCost();
}

/**
 * Validate input parameters
 *
 * @return true if inputs are valid, false otherwise
 */
bool SolverDP::validateInputs() {
    if (N == 0) {
        cout << "Erreur: N est égal à 0" << endl;
        return false;
    }

    if (K == 0) {
        cout << "Erreur: K est égal à 0" << endl;
        return false;
    }

    return true;
}

/**
 * Initialize the dynamic programming matrix
 */
void SolverDP::initializeMatrix() {
    matrixDP.initMatrix(N, K);
    cout << "Matrice initialisée avec " << matrixDP.getRows() << " lignes et " << matrixDP.getCols() << " colonnes" << endl;
}

/**
 * Fill the dynamic programming matrix with optimal costs
 */
void SolverDP::fillDPMatrix(vector<double>& v) {
    for (uint k = 1; k < K && k < matrixDP.getRows(); k++) {

        for (uint n = k; n < N; n++) {
            // Get costs for clusters ending at point n
            clusterCostsBefore(n, v);

            // Find and store the optimal cost for this subproblem
            OptimalSplit optSplit = findOptimalSplit(k, n, v);

            if (k < matrixDP.getRows() && n < matrixDP.getCols()) {
                matrixDP.setElement(k, n, optSplit.cost);
            }
        }
    }
}

/**
 * Find the optimal split point for a given configuration
 *
 * @param k Current cluster index
 * @param n Current point index
 * @param v Vector of costs for different cluster lengths
 * @return Optimal split information (cost and split point)
 */
SolverDP::OptimalSplit SolverDP::findOptimalSplit(uint k, uint n, const vector<double>& v) {
    OptimalSplit result;
    result.cost = std::numeric_limits<double>::max();
    result.splitPoint = 0;
    result.isValid = false;

    // Try all possible split points
    for (uint split = k-1; split < n; split++) {
        if (k-1 < matrixDP.getRows() && split < matrixDP.getCols() &&
            n-split-1 < v.size() && n-split-1 >= 0) {

            double cost = matrixDP.getElement(k-1, split) + v[n-split-1];
            if (cost < result.cost) {
                result.cost = cost;
                result.splitPoint = split;
                result.isValid = true;
            }
        }
    }

    return result;
}

/**
 * Build the solution by backtracking from the DP matrix
 */
void SolverDP::buildSolutionFromMatrix() {
    solutionInterval.clear();
    uint remainingPoints = N-1;

    // Work backwards to find optimal split points
    for (int k = K-1; k > 0; k--) {
        vector<double> v(N, 0.0);
        clusterCostsBefore(remainingPoints, v);

        OptimalSplit optSplit = findOptimalSplit(k, remainingPoints, v);

        // Add this interval to our solution only if we found a valid split
        if (optSplit.isValid) {
            solutionInterval.push_back(make_pair(optSplit.splitPoint+1, remainingPoints));
            remainingPoints = optSplit.splitPoint;
        } else {
            break;
        }
    }

    // Add the first cluster if needed
    if (remainingPoints > 0) {
        solutionInterval.push_back(make_pair(0, remainingPoints));
    }

    // Reverse the intervals to get them in ascending order
    reverse(solutionInterval.begin(), solutionInterval.end());
}

/**
 * Calculate the final cost of the solution
 */
void SolverDP::calculateFinalCost() {
    if (K-1 < matrixDP.getRows() && N-1 < matrixDP.getCols()) {
        solutionCost = matrixDP.getElement(K-1, N-1);
    } else {
        solutionCost = std::numeric_limits<double>::max();
    }
}

/**
 * Check if the matrix is available (not deleted)
 *
 * @return true if matrix is available, false otherwise
 */
bool SolverDP::isMatrixAvailable() {
    if (matrixDP.getRows() == 0 || matrixDP.getCols() == 0) {
        cout << "Matrice non disponible (déjà libérée)" << endl;
        return false;
    }
    return true;
}