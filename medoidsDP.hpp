#pragma once

#include "solverDP.hpp"


class MedoidsDP : public SolverDP {

protected:
    void clusterCostsBefore(uint i, vector<double> & v) override;
    void clusterCostsFromBeginning(vector<double>  & v) override;


private:
    // Helper functions for cost calculations
    void computeInitialCosts(uint start, uint end, vector<vector<double>> &d);
    void updateCostsRecursively(uint start, uint end, uint lastPoint, uint len, vector<vector<double>> &d);
    double findMinimumCost(uint start, uint end, uint len, const vector<vector<double>> &d);
};
