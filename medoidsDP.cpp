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
*/
void MedoidsDP::calculateClusterCosts(uint end, vector<double> &v, bool isFromBeginning) {
    // Initialize the matrix d to store costs for each potential medoid and length
    d.initMatrix(N, N);  // Using the member variable d

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
            computeInitialCosts(clusterStart, clusterEnd);  // Removed d parameter
        }
            // Apply recurrence relation for longer clusters
        else {
            updateCostsIteratively(clusterStart, clusterEnd, lastPoint, len);  // Removed d parameter
        }

        // Find the medoid that minimizes cost for this length
        v[len] = findMinimumCost(clusterStart, clusterEnd, len);  // Removed d parameter
    }
}

/**
* Computes initial costs for each potential medoid in a cluster of length 2
*/
void MedoidsDP::computeInitialCosts(uint start, uint end) {
    for(uint medoid = start; medoid <= end; medoid++) {
        for(uint otherPoint = start; otherPoint <= end; otherPoint++) {
            if(otherPoint != medoid) {
                d.setElement(medoid, 1, squaredDistance(otherPoint, medoid));
            }
        }
    }
}

/**
* Updates costs using the recurrence relation for longer clusters
*/
void MedoidsDP::updateCostsIteratively(uint start, uint end, uint lastPoint, uint len) {
    // For each potential medoid in the current interval
    for (uint medoid = start; medoid <= end; medoid++) {
        if (medoid < lastPoint) {
            // Medoid was already in the previous interval
            // Use recurrence relation: add cost of the new point only
            d.setElement(medoid, len, d.getElement(medoid, len-1) + squaredDistance(lastPoint, medoid));
        } else {
            // New potential medoid (including lastPoint itself)
            // Must recalculate the full cost
            d.setElement(medoid, len, 0);

            // Sum distances from all other points in the interval to this medoid
            for (uint point = start; point < end; point++) {
                d.setElement(medoid, len, d.getElement(medoid, len) + squaredDistance(point, medoid));
            }
        }
    }
}

/**
* Finds the minimum cost among all potential medoids for a given cluster length
*/
double MedoidsDP::findMinimumCost(uint start, uint end, uint len) {
    double minCost = std::numeric_limits<double>::max();
    for(uint c = start; c <= end; c++) {
        if(d.getElement(c, len) < minCost) {
            minCost = d.getElement(c, len);
        }
    }
    return minCost;
}