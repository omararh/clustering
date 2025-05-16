#pragma once

#include "solverDP.hpp"


class MedoidsDP : public SolverDP {

protected:
    void clusterCostsBefore(uint i, vector<double> & v) override;
    void clusterCostsFromBeginning(vector<double>  & v) override;
};