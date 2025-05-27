#include "medoidsDP.hpp"
#include <limits>
#include <iostream>

/**
 * Computes cluster costs for all possible clusters ending at index i
 * Used by the DP algorithm to calculate the cost of placing the last cluster
 * at different positions ending at point i
 *
 * @param i The ending index for all clusters to compute
 * @param v Output vector where v[j] = cost of cluster with j+1 points ending at i
 */
void MedoidsDP::clusterCostsBefore(uint i, vector<double>& v) {
    std::cout << "DEBUG clusterCostsBefore: i=" << i << " v.size()=" << v.size() << std::endl;

    std::fill(v.begin(), v.end(), std::numeric_limits<double>::max());

    if (v.size() == 0 || i >= N) return;

    for (uint numPoints = 1; numPoints <= i + 1 && numPoints <= v.size(); numPoints++) {
        uint clusterStart = i - numPoints + 1;
        uint clusterEnd = i;

        if (clusterStart > clusterEnd || clusterEnd >= N) {
            continue;
        }

        double cost = calculateClusterCost(clusterStart, clusterEnd);
        v[numPoints - 1] = cost;

        std::cout << "DEBUG: v[" << (numPoints - 1) << "] = " << cost
                  << " (cluster [" << clusterStart << ", " << clusterEnd << "], "
                  << numPoints << " points)" << std::endl;
    }
}

/**
 * Computes cluster costs for all possible clusters starting from index 0
 * Used by the DP algorithm to initialize the first row of the DP matrix
 *
 * @param v Output vector where v[j] = cost of cluster with first j+1 points
 */
void MedoidsDP::clusterCostsFromBeginning(vector<double>& v) {
    std::cout << "DEBUG clusterCostsFromBeginning: v.size()=" << v.size() << std::endl;

    std::fill(v.begin(), v.end(), std::numeric_limits<double>::max());

    if (v.size() == 0) return;

    for (uint numPoints = 1; numPoints <= N && numPoints <= v.size(); numPoints++) {
        uint clusterStart = 0;
        uint clusterEnd = numPoints - 1;

        if (clusterEnd >= N) {
            continue;
        }

        double cost = calculateClusterCost(clusterStart, clusterEnd);
        v[numPoints - 1] = cost;

        std::cout << "DEBUG: v[" << (numPoints - 1) << "] = " << cost
                  << " (cluster [" << clusterStart << ", " << clusterEnd << "], "
                  << numPoints << " points)" << std::endl;
    }
}

/**
 * Calculates the optimal k-medoids cost for a cluster of consecutive points
 * Tests each point in the range as a potential medoid and returns the minimum cost
 * Cost is the sum of squared Euclidean distances from all points to the medoid
 *
 * @param start Starting index of the cluster (inclusive)
 * @param end Ending index of the cluster (inclusive)
 * @return Minimum cost achieved by the optimal medoid selection
 */
double MedoidsDP::calculateClusterCost(uint start, uint end) const {
    if (start > end) return 0.0;
    if (start == end) return 0.0;

    double minCost = std::numeric_limits<double>::max();

    std::cout << "    calculateClusterCost [" << start << ", " << end << "]:" << std::endl;

    for (uint medoid = start; medoid <= end; medoid++) {
        double cost = 0.0;
        for (uint i = start; i <= end; i++) {
            if (i != medoid) {
                double dist = squaredDistance(i, medoid);
                cost += dist;
                std::cout << "      dist(" << i << ", " << medoid << ") = " << dist << std::endl;
            }
        }
        std::cout << "    medoid " << medoid << ": cost = " << cost << std::endl;
        minCost = std::min(minCost, cost);
    }

    std::cout << "    --> minCost = " << minCost << std::endl;
    return minCost;
}