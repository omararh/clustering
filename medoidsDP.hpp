#pragma once

#include "solverDP.hpp"
#include "matrixDouble.hpp"

class MedoidsDP : public SolverDP {

protected:
    void clusterCostsBefore(uint i, vector<double> & v) override;
    void clusterCostsFromBeginning(vector<double>  & v) override;

private:
    MatrixDouble d;

    // Helper functions for cost calculations
    void calculateClusterCosts(uint end, vector<double> &v, bool isFromBeginning);
    void computeInitialCosts(uint start, uint end);
    void updateCostsIteratively(uint start, uint end, uint lastPoint, uint len);
    double findMinimumCost(uint start, uint end, uint len);
};
