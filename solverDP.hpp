#pragma once
#include "matrixDouble.hpp"
#include "solverInterval.hpp"

class SolverDP : public SolverInterval {
public:
    void solve();
    void printMatrixDP();
    void printFinalCosts(string sep);
    MatrixDouble getMatrix() { return matrixDP; }
    double calculateRealClusterCost() const;

protected:
    MatrixDouble matrixDP;

    void fillFirstLine(vector<double>& v);
    virtual void clusterCostsBefore(uint i, vector<double>& v) = 0;
    virtual void clusterCostsFromBeginning(vector<double>& v) = 0;

    bool validateInputs();
    void initializeMatrix();
    void fillDPMatrix(vector<double>& v);
    void buildSolutionFromMatrix();
    void calculateFinalCost();
    bool isMatrixAvailable();

    struct OptimalSplit {
        double cost;
        uint splitPoint;
        bool isValid;
    };

    OptimalSplit findOptimalSplit(uint k, uint n, const vector<double>& v);
};