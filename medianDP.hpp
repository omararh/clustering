    #pragma once
#include "solverDP.hpp"

class MedianDP : public SolverDP {
protected:
    void clusterCostsBefore(uint i, vector<double>& v) override;
    void clusterCostsFromBeginning(vector<double>& v) override;

private:
    double calculateClusterCost(uint start, uint end) const;
};
