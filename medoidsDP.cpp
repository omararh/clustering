#include "medoidsDP.hpp"
#include <limits>
#include <vector>

/**
* Calculates the optimal costs for all possible clusters ending at index i
*
* Uses a dynamic programming approach to efficiently compute costs:
* - For each possible length of cluster ending at i
* - Tests all potential medoids
* - Uses previously calculated costs where possible
* - Stores the minimal cost for each length in vector v
*
* @param i The ending index of the clusters
* @param v Output vector to store computed costs for each length
*/
void MedoidsDP::clusterCostsBefore(uint i, vector<double> & v) {
    // Matrix to store costs for each potential medoid and length
    // d[c][len] = cost of cluster [i-len, i] with medoid c
    vector<vector<double>> d(N, vector<double>(N, 0.0));

    // Process each possible cluster length
    for(uint len = 0; len < v.size(); len++) {
        // Skip invalid lengths
        if(len > i) {
            v[len] = std::numeric_limits<double>::max();
            continue;
        }

        uint start = i - len;
        uint end = i;

        // Handle single-point cluster case
        if(len == 0) {
            v[len] = 0;
            continue;
        }

        // Initialize costs for clusters of length 2
        if(len == 1) {
            computeInitialCosts(start, end, d);
        }
            // Apply recurrence relation for longer clusters
        else {
            updateCostsRecursively(start, end, i, len, d);
        }

        // Find the medoid that minimizes cost for this length
        v[len] = findMinimumCost(start, end, len, d);
    }
}

/**
* Calculates the optimal costs for all possible clusters starting from index 0
*
* Similar to clusterCostsBefore but specialized for clusters starting at the beginning
* Uses the same dynamic programming approach for efficiency
*
* @param v Output vector to store computed costs for each length
*/
void MedoidsDP::clusterCostsFromBeginning(vector<double> & v) {
    // Matrix to store costs for each potential medoid and length
    // d[c][len] = cost of cluster [0, len] with medoid c
    vector<vector<double>> d(N, vector<double>(N, 0.0));

    // Process each possible cluster length
    for(uint len = 0; len < v.size(); len++) {
        // Skip invalid lengths
        if(len >= N) {
            v[len] = std::numeric_limits<double>::max();
            continue;
        }

        uint start = 0;
        uint end = len;

        // Handle single-point cluster case
        if(len == 0) {
            v[len] = 0;
            continue;
        }

        // Initialize costs for clusters of length 2
        if(len == 1) {
            computeInitialCosts(start, end, d);
        }
            // Apply recurrence relation for longer clusters
        else {
            updateCostsRecursively(start, end, len, len, d);
        }

        // Find the medoid that minimizes cost for this length
        v[len] = findMinimumCost(start, end, len, d);
    }
}

/**
* Computes initial costs for each potential medoid in a cluster of length 2
*
* @param start Start index of the cluster
* @param end End index of the cluster
* @param d Cost matrix to update
*/
void MedoidsDP::computeInitialCosts(uint start, uint end, vector<vector<double>> &d) {
    for(uint c = start; c <= end; c++) {
        for(uint j = start; j <= end; j++) {
            if(j != c) {
                d[c][1] = squaredDistance(j, c);
            }
        }
    }
}

/**
* Updates costs using the recurrence relation for longer clusters
*
* @param start Start index of the cluster
* @param end End index of the cluster
* @param lastPoint The last point added to the cluster
* @param len Current length of the cluster
* @param d Cost matrix to update
*/
void MedoidsDP::updateCostsRecursively(uint start, uint end, uint lastPoint, uint len, vector<vector<double>> &d) {
    for(uint c = start; c <= end; c++) {
        if(c < lastPoint) {
            // Medoid was in the previous interval - add cost of new point
            d[c][len] = d[c][len-1] + squaredDistance(lastPoint, c);
        } else {
            // New potential medoid - recalculate
            d[c][len] = 0;
            for(uint j = start; j < end; j++) {
                d[c][len] += squaredDistance(j, c);
            }
        }
    }
}

/**
* Finds the minimum cost among all potential medoids for a given cluster length
*
* @param start Start index of the cluster
* @param end End index of the cluster
* @param len Length of the cluster
* @param d Cost matrix with all medoid costs
* @return The minimum cost for this cluster
*/
double MedoidsDP::findMinimumCost(uint start, uint end, uint len, const vector<vector<double>> &d) {
    double minCost = std::numeric_limits<double>::max();
    for(uint c = start; c <= end; c++) {
        if(d[c][len] < minCost) {
            minCost = d[c][len];
        }
    }
    return minCost;
}