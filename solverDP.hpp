#pragma once

#include "matrixDouble.hpp"
#include "solverInterval.hpp"
//#include <utility>

/**
 * @class SolverDP
 * @brief Dynamic Programming implementation for clustering optimization
 *
 * The class uses a matrix (matrixDP) to store optimal costs for subproblems,
 * where each cell represents the best clustering cost for a specific number of
 * clusters and points. This memoization avoids recalculating the same subproblems
 * multiple times and enables efficient construction of the entire Pareto front.
 *
 * Key features:
 * - Solves the multi-objective optimization problem by generating Pareto-optimal solutions
 * - Works with intervals of sorted points for efficient computation
 * - Uses dynamic programming to build optimal solutions from smaller subproblems
 * - Constructs a matrix of optimal costs for different clustering configurations
 *
 * Derived classes must implement:
 * - clusterCostsBefore(): Calculate costs for clusters ending at a specific point
 * - clusterCostsFromBeginning(): Calculate costs for clusters starting from the first point
 */
class SolverDP : public SolverInterval {

protected:

    MatrixDouble matrixDP;
    uint lastIndex;
    /**
     * @brief Initializes the first row of the DP matrix with costs for single-cluster configurations and stores the results in the matrix
     */
    void fillFirstLine();
    /**
    * @brief Calculates optimal costs for all possible clusters ending at index i
    */
    virtual void clusterCostsBefore(uint i, vector<double> & v)=0;
    /**
    * @brief Calculates optimal costs for all possible clusters starting from the beginning
    */
    virtual void clusterCostsFromBeginning(vector<double>  & v)=0;


public:

    void solve();
    void printMatrixDP();
    void printFinalCosts(string sep);
};

