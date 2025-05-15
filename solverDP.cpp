#include <iostream>
#include <limits>
#include "solverDP.hpp"

void SolverDP::printFinalCosts(string sep) {
    cout << endl;
    for(uint i = 0; i < K; i++){
        cout << matrixDP.getElement(i, N-1) << sep;
    }
    cout << endl;
}

void SolverDP::printMatrixDP() {
    cout << endl;
    for(int i = K-1; i >= 0; i--) {
        for(uint n = 0; n < N; n++)
            cout << matrixDP.getElement(i, n) << " ";
        cout << endl;
    }
    cout << endl;
}

void SolverDP::fillFirstLine() {
    vector<double> v;
    v.resize(N);

    // Calculate costs for clusters that start from the beginning of the array
    clusterCostsFromBeginning(v);

    // Fill the first row of the matrix with these costs
    for(uint i = 0; i < N; i++) {
        matrixDP.setElement(0, i, v[i]);
    }
}

void SolverDP::solve() {
    // Ensure points are sorted on the first dimension
    resort();

    // Initialize the DP matrix with dimensions N×(K-1)
    matrixDP.initMatrix(N, K);

    // Fill the first line representing single cluster costs
    fillFirstLine();

    // Fill the rest of the matrix using dynamic programming
    for(uint k = 1; k < K; k++) {
        vector<double> v;
        v.resize(N);

        for(uint n = k; n < N; n++) {
            // Get costs for clusters ending at point n
            clusterCostsBefore(n, v);

            // Find the optimal split point
            double minCost = std::numeric_limits<double>::max();
            uint bestSplit = 0;

            // Try all possible split points
            for(uint split = k-1; split < n; split++) {
                double cost = matrixDP.getElement(k-1, split) + v[n-split-1];
                if(cost < minCost) {
                    minCost = cost;
                    bestSplit = split;
                }
            }

            // Store the optimal cost for subproblem
            matrixDP.setElement(k, n, minCost);
        }
    }

    // Backtracking operations to build the solution
    solutionInterval.clear();

    // Start from the bottom-right of the matrix
    uint remainingPoints = N-1;

    // Work backwards to find optimal split points
    for(int k = K-1; k > 0; k--) {
        double minCost = std::numeric_limits<double>::max();
        uint bestSplit = 0;

        // Find the best split point for this cluster
        for(uint split = k-1; split < remainingPoints; split++) {
            vector<double> v;
            v.resize(N);
            clusterCostsBefore(remainingPoints, v);

            double cost = matrixDP.getElement(k-1, split) + v[remainingPoints-split-1];
            if(cost < minCost) {
                minCost = cost;
                bestSplit = split;
            }
        }

        // Add this interval to our solution
        solutionInterval.push_back(make_pair(bestSplit+1, remainingPoints));
        remainingPoints = bestSplit;
    }

    // Add the first cluster
    if(remainingPoints > 0) {
        solutionInterval.push_back(make_pair(0, remainingPoints));
    }

    // Reverse the intervals to get them in ascending order
    reverse(solutionInterval.begin(), solutionInterval.end());

    // Compute final solution from intervals
    computeSolutionFromIntervals();

    // Calculate the final cost
    solutionCost = matrixDP.getElement(K-1, N-1);

    // Clean up memory
    matrixDP.deleteMatrix();
}