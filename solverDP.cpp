#include "solverDP.hpp"
#include <iostream>
#include <limits>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

void SolverDP::solve() {
    if (!validateInputs()) return;

#ifdef _OPENMP
    std::cout << "OpenMP disponible avec " << omp_get_max_threads() << " threads" << std::endl;
#endif

    resort(); // Trier les points
    initializeMatrix();

    vector<double> v(N, 0.0);
    fillFirstLine(v);
    fillDPMatrix(v);
    buildSolutionFromMatrix();
    computeSolutionFromIntervals();
    calculateFinalCost();
}

bool SolverDP::validateInputs() {
    return N > 0 && K > 0;
}

void SolverDP::initializeMatrix() {
    matrixDP.initMatrix(K, N); // K lignes, N colonnes

    // Parallélisation de l'initialisation pour les grandes matrices
    bool useParallel = (K * N > 10000);

#pragma omp parallel for collapse(2) if(useParallel)
    for (size_t k = 0; k < K; k++) {
        for (size_t n = 0; n < N; n++) {
            matrixDP.setElement(k, n, std::numeric_limits<double>::max());
        }
    }

    std::cout << "Matrice initialisée avec " << matrixDP.getRows()
              << " lignes et " << matrixDP.getCols() << " colonnes" << std::endl;
}

void SolverDP::fillFirstLine(vector<double>& v) {
    if (N == 0) return;

    std::cout << "DEBUG fillFirstLine: calling clusterCostsFromBeginning" << std::endl;
    clusterCostsFromBeginning(v);

    std::cout << "DEBUG fillFirstLine: v = ";
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;

    // Remplir la première ligne séquentiellement (dépendances)
    for (uint n = 0; n < N && n < matrixDP.getCols(); n++) {
        if (n < v.size()) {
            matrixDP.setElement(0, n, v[n]);
            std::cout << "DEBUG: matrixDP[0][" << n << "] = " << v[n] << std::endl;
        }
    }
}

void SolverDP::fillDPMatrix(vector<double>& v) {
    // La DP impose des dépendances entre lignes, mais les colonnes d'une même ligne
    // peuvent être calculées en parallèle
    for (uint k = 1; k < K && k < matrixDP.getRows(); k++) {

        // Parallélisation des colonnes d'une même ligne
        bool useParallel = (N > 50);

#pragma omp parallel if(useParallel)
        {
            // Chaque thread a son propre vecteur v local
            vector<double> local_v(N, 0.0);

#pragma omp for schedule(dynamic)
            for (uint n = k; n < N; n++) {
                // Calculer les coûts pour cette position
                clusterCostsBefore(n, local_v);
                OptimalSplit optSplit = findOptimalSplit(k, n, local_v);

                if (k < matrixDP.getRows() && n < matrixDP.getCols()) {
/*
#pragma omp critical
                    {
*/
                        matrixDP.setElement(k, n, optSplit.cost);
/*
                        std::cout << "DEBUG: matrixDP[" << k << "][" << n << "] = " << optSplit.cost
                                  << " (split=" << optSplit.splitPoint << ")" << std::endl;
                     }
*/
                }
            }
        }
    }
}

SolverDP::OptimalSplit SolverDP::findOptimalSplit(uint k, uint n, const vector<double>& v) {
    OptimalSplit result;
    result.cost = std::numeric_limits<double>::max();
    result.splitPoint = 0;
    result.isValid = false;

    std::cout << "    findOptimalSplit: k=" << k << " n=" << n << " v.size()=" << v.size() << std::endl;

    // Parallélisation de la recherche du split optimal pour les grandes instances
    uint numSplits = n - (k-1);
    bool useParallel = (numSplits > 20);

    // Variables pour la réduction personnalisée
    double bestCost = std::numeric_limits<double>::max();
    uint bestSplit = 0;
    bool foundValid = false;

#pragma omp parallel if(useParallel)
    {
        double localBestCost = std::numeric_limits<double>::max();
        uint localBestSplit = 0;
        bool localFoundValid = false;

#pragma omp for
        for (uint split = k-1; split < n; split++) {
            if (split < matrixDP.getCols()) {
                double leftCost = matrixDP.getElement(k-1, split);
                uint clusterSize = n - split;

                if (clusterSize > 0 && clusterSize - 1 < v.size()) {
                    double rightCost = v[clusterSize - 1];
                    double totalCost = leftCost + rightCost;

/*
#pragma omp critical
                    {
                        std::cout << "      split=" << split << " leftCost=" << leftCost
                                  << " clusterSize=" << clusterSize << " rightCost=" << rightCost
                                  << " totalCost=" << totalCost << std::endl;
                    }
*/

                    if (totalCost < localBestCost &&
                        leftCost != std::numeric_limits<double>::max() &&
                        rightCost != std::numeric_limits<double>::max()) {
                        localBestCost = totalCost;
                        localBestSplit = split;
                        localFoundValid = true;
                    }
                }
            }
        }

        // Réduction manuelle pour trouver le meilleur résultat global
#pragma omp critical
        {
            if (localFoundValid && localBestCost < bestCost) {
                bestCost = localBestCost;
                bestSplit = localBestSplit;
                foundValid = true;
            }
        }
    }

    if (foundValid) {
        result.cost = bestCost;
        result.splitPoint = bestSplit;
        result.isValid = true;
    }

    std::cout << "    --> bestCost=" << result.cost << " bestSplit=" << result.splitPoint << std::endl;
    return result;
}

void SolverDP::buildSolutionFromMatrix() {
    solutionInterval.clear();

    // Le backtracking est séquentiel par nature (dépendances)
    uint currentK = K - 1;
    uint currentN = N - 1;
    vector<double> v(N, 0.0);

    while (currentK > 0) {
        clusterCostsBefore(currentN, v);
        OptimalSplit optSplit = findOptimalSplit(currentK, currentN, v);

        if (optSplit.isValid) {
            solutionInterval.push_back(make_pair(optSplit.splitPoint + 1, currentN));
            currentN = optSplit.splitPoint;
            currentK--;
        } else {
            break;
        }
    }

    if (currentK == 0) {
        solutionInterval.push_back(make_pair(0, currentN));
    }

    reverse(solutionInterval.begin(), solutionInterval.end());

    std::cout << "\nIntervalles reconstruits:" << std::endl;
    for (size_t i = 0; i < solutionInterval.size(); i++) {
        std::cout << "Cluster " << i+1 << ": [" << solutionInterval[i].first
                  << ", " << solutionInterval[i].second << "]" << std::endl;
    }
}

void SolverDP::calculateFinalCost() {
    if (K-1 < matrixDP.getRows() && N-1 < matrixDP.getCols()) {
        solutionCost = matrixDP.getElement(K-1, N-1);
    } else {
        solutionCost = std::numeric_limits<double>::max();
    }
}

double SolverDP::calculateRealClusterCost() const {
    double totalCost = 0.0;

    // Parallélisation du calcul de vérification
#pragma omp parallel for reduction(+:totalCost) if(K > 4)
    for (size_t k = 1; k <= K; k++) {
        vector<size_t> clusterPoints;
        for (size_t i = 0; i < N; i++) {
            if (solution[i] == k) {
                clusterPoints.push_back(i);
            }
        }

        if (clusterPoints.size() <= 1) continue;

        double minCost = std::numeric_limits<double>::max();
        for (size_t medoidIdx : clusterPoints) {
            double cost = 0.0;
            for (size_t pointIdx : clusterPoints) {
                if (pointIdx != medoidIdx) {
                    cost += squaredDistance(pointIdx, medoidIdx);
                }
            }
            minCost = std::min(minCost, cost);
        }
        totalCost += minCost;
    }
    return totalCost;
}

bool SolverDP::isMatrixAvailable() {
    return matrixDP.getRows() > 0 && matrixDP.getCols() > 0;
}

void SolverDP::printMatrixDP() {
    if (!isMatrixAvailable()) return;

    std::cout << "\n=== MATRICE DP ===" << std::endl;
    for (int k = K-1; k >= 0; k--) {
        std::cout << "k=" << k << ": ";
        for (uint n = 0; n < N; n++) {
            if (k < static_cast<int>(matrixDP.getRows()) && n < matrixDP.getCols()) {
                double val = matrixDP.getElement(k, n);
                if (val == std::numeric_limits<double>::max()) {
                    std::cout << "∞ ";
                } else {
                    std::cout << val << " ";
                }
            } else {
                std::cout << "NA ";
            }
        }
        std::cout << std::endl;
    }
}

void SolverDP::printFinalCosts(string sep) {
    if (!isMatrixAvailable()) return;

    for (uint k = 0; k < K; k++) {
        if (k < matrixDP.getRows() && N-1 < matrixDP.getCols()) {
            double val = matrixDP.getElement(k, N-1);
            if (val == std::numeric_limits<double>::max()) {
                std::cout << "avec " << k + 1 << " clusters = ∞" << sep;
            } else {
                std::cout << "avec " << k + 1 << " clusters = " << val << sep;
            }
        } else {
            std::cout << "NA" << sep;
        }
    }
    std::cout << std::endl;
}