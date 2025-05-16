#include <iostream>
#include <limits>
#include "solverDP.hpp"
#include <vector>

using namespace std;

void SolverDP::printFinalCosts(string sep) {
    cout << endl;

    // Vérifier si la matrice a été supprimée
    if (matrixDP.getRows() == 0 || matrixDP.getCols() == 0) {
        cout << "Matrice non disponible (déjà libérée)" << endl;
        return;
    }

    for(uint i = 0; i < K; i++){
        if (i < matrixDP.getRows() && N-1 < matrixDP.getCols()) {
            cout << matrixDP.getElement(i, N-1) << sep;
        } else {
            cout << "NA" << sep;
        }
    }
    cout << endl;
}

void SolverDP::printMatrixDP() {
    cout << endl;

    // Vérifier si la matrice a été supprimée
    if (matrixDP.getRows() == 0 || matrixDP.getCols() == 0) {
        cout << "Matrice non disponible (déjà libérée)" << endl;
        return;
    }

    for(int i = K-1; i >= 0; i--) {
        for(uint n = 0; n < N; n++) {
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

void SolverDP::fillFirstLine() {
    if (N == 0) return;

    vector<double> v(N, 0.0);

    // Calculate costs for clusters that start from the beginning of the array
    clusterCostsFromBeginning(v);

    // Fill the first row of the matrix with these costs
    for(uint i = 0; i < N && i < matrixDP.getCols(); i++) {
        matrixDP.setElement(0, i, v[i]);
    }
}

void SolverDP::solve() {
    // Vérifications de base
    if (N == 0) {
        cout << "Erreur: N est égal à 0" << endl;
        return;
    }

    if (K == 0) {
        cout << "Erreur: K est égal à 0" << endl;
        return;
    }

    // Ensure points are sorted on the first dimension
    resort();

    // Débug info
    cout << "Initialisation de la matrice avec K=" << K << " et N=" << N << endl;

    // Correction: Initialiser la matrice avec K lignes et N colonnes
    matrixDP.initMatrix(N, K);

    // Débug info
    cout << "Matrice initialisée avec " << matrixDP.getRows() << " lignes et " << matrixDP.getCols() << " colonnes" << endl;

    // Fill the first line representing single cluster costs
    fillFirstLine();

    // Fill the rest of the matrix using dynamic programming
    for(uint k = 1; k < K && k < matrixDP.getRows(); k++) {
        vector<double> v(N, 0.0);

        for(uint n = k; n < N; n++) {
            // Get costs for clusters ending at point n
            clusterCostsBefore(n, v);

            // Find the optimal split point
            double minCost = std::numeric_limits<double>::max();
            uint bestSplit = 0;
            bool foundValidSplit = false;

            // Try all possible split points
            for(uint split = k-1; split < n; split++) {
                if (k-1 < matrixDP.getRows() && split < matrixDP.getCols() &&
                    n-split-1 < v.size() && n-split-1 >= 0) {
                    double cost = matrixDP.getElement(k-1, split) + v[n-split-1];
                    if(cost < minCost) {
                        minCost = cost;
                        bestSplit = split;
                        foundValidSplit = true;
                    }
                }
            }

            // Store the optimal cost for subproblem
            if (k < matrixDP.getRows() && n < matrixDP.getCols() && foundValidSplit) {
                matrixDP.setElement(k, n, minCost);
            } else if (k < matrixDP.getRows() && n < matrixDP.getCols()) {
                matrixDP.setElement(k, n, std::numeric_limits<double>::max());
            }
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
        bool foundValidSplit = false;

        // Find the best split point for this cluster
        for(uint split = k-1; split < remainingPoints; split++) {
            vector<double> v(N, 0.0);
            clusterCostsBefore(remainingPoints, v);

            if (k-1 < matrixDP.getRows() && split < matrixDP.getCols() &&
                remainingPoints-split-1 < v.size() && remainingPoints-split-1 >= 0) {
                double cost = matrixDP.getElement(k-1, split) + v[remainingPoints-split-1];
                if(cost < minCost) {
                    minCost = cost;
                    bestSplit = split;
                    foundValidSplit = true;
                }
            }
        }

        // Add this interval to our solution only if we found a valid split
        if (foundValidSplit) {
            solutionInterval.push_back(make_pair(bestSplit+1, remainingPoints));
            remainingPoints = bestSplit;
        } else {
            // Si on ne trouve pas de point de séparation valide, arrêter la boucle
            break;
        }
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
    if (K-1 < matrixDP.getRows() && N-1 < matrixDP.getCols()) {
        solutionCost = matrixDP.getElement(K-1, N-1);
    } else {
        solutionCost = std::numeric_limits<double>::max();
    }
}