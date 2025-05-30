#pragma once
#include "solverDP.hpp"

class MedoidsDP : public SolverDP {
protected:
    void clusterCostsBefore(uint i, vector<double>& v) override;
    void clusterCostsFromBeginning(vector<double>& v) override;

    double calculateClusterCost(uint start, uint end) const;

private:
    MatrixDouble d;
};