#include "medoidsDP.hpp"
#include <limits>
#include <vector>

/**
* Calculates the optimal costs for all possible clusters ending at index i
*
* @param i The ending index of the clusters
* @param v Output vector to store computed costs for each length
*/
void MedoidsDP::clusterCostsBefore(uint i, vector<double> &v) {
    calculateClusterCosts(i, v, false);
}

/**
* Calculates the optimal costs for all possible clusters starting from index 0
*
* @param v Output vector to store computed costs for each length
*/
void MedoidsDP::clusterCostsFromBeginning(vector<double> &v) {
    calculateClusterCosts(0, v, true);
}

/**
* Base method to calculate optimal costs for clusters with specified bounds
*
* @param start Fixed starting index of clusters
* @param end Fixed ending index of clusters
* @param v Output vector to store computed costs for each length
* @param isFromBeginning Flag indicating if clusters start from index 0
*/
void MedoidsDP::calculateClusterCosts(uint end, vector<double> &v, bool isFromBeginning) {
    // Matrix to store costs for each potential medoid and length
    vector<vector<double>> d(N, vector<double>(N, 0.0));

    // Handle single-point cluster case outside the loop
    if (v.size() > 0) {
        v[0] = 0;
    }

    // Process each possible cluster length, starting from 1
    for (uint len = 1; len < v.size(); len++) {
        // Skip invalid lengths
        if ((isFromBeginning && len >= N) || (!isFromBeginning && len > end)) {
            v[len] = std::numeric_limits<double>::max();
            continue;
        }

        // Determine current cluster bounds
        uint clusterStart = isFromBeginning ? 0 : end - len;
        uint clusterEnd = end;
        uint lastPoint = isFromBeginning ? len : end;

        // Initialize costs for clusters of length 2
        if (len == 1) {
            computeInitialCosts(clusterStart, clusterEnd, d);
        }
        // Apply recurrence relation for longer clusters
        else {
            updateCostsIteratively(clusterStart, clusterEnd, lastPoint, len, d);
        }

        // Find the medoid that minimizes cost for this length
        v[len] = findMinimumCost(clusterStart, clusterEnd, len, d);
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
    for(uint medoid = start; medoid <= end; medoid++) {
        for(uint otherPoint = start; otherPoint <= end; otherPoint++) {
            if(otherPoint != medoid) {
                d[medoid][1] = squaredDistance(otherPoint, medoid);
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
void MedoidsDP::updateCostsIteratively(uint start, uint end, uint lastPoint, uint len, vector<vector<double>> &d) {
    // For each potential medoid in the current interval
    for (uint medoid = start; medoid <= end; medoid++) {
        if (medoid < lastPoint) {
            // Medoid was already in the previous interval
            // Use recurrence relation: add cost of the new point only
            d[medoid][len] = d[medoid][len-1] + squaredDistance(lastPoint, medoid);
        } else {
            // New potential medoid (including lastPoint itself)
            // Must recalculate the full cost
            d[medoid][len] = 0;

            // Sum distances from all other points in the interval to this medoid
            for (uint point = start; point < end; point++) {
                d[medoid][len] += squaredDistance(point, medoid);
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