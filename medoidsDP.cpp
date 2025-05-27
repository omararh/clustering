#include "medoidsDP.hpp"
#include <limits>
#include <iostream>
#ifdef _OPENMP
#include <omp.h>
#endif

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

    // Parallélisation de la boucle principale si suffisamment de calculs
    bool useParallel = (i > 50); // Seuil pour éviter l'overhead sur petites instances

#pragma omp parallel for if(useParallel) schedule(dynamic)
    for (uint numPoints = 1; numPoints <= i + 1 && numPoints <= v.size(); numPoints++) {
        uint clusterStart = i - numPoints + 1;
        uint clusterEnd = i;

        if (clusterStart > clusterEnd || clusterEnd >= N) {
            continue;
        }

        double cost = calculateClusterCost(clusterStart, clusterEnd);
        v[numPoints - 1] = cost;

#pragma omp critical
        {
            std::cout << "DEBUG: v[" << (numPoints - 1) << "] = " << cost
                      << " (cluster [" << clusterStart << ", " << clusterEnd << "], "
                      << numPoints << " points)" << std::endl;
        }
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

    // Parallélisation pour les grandes instances
    bool useParallel = (N > 100);

#pragma omp parallel for if(useParallel) schedule(dynamic)
    for (uint numPoints = 1; numPoints <= N && numPoints <= v.size(); numPoints++) {
        uint clusterStart = 0;
        uint clusterEnd = numPoints - 1;

        if (clusterEnd >= N) {
            continue;
        }

        double cost = calculateClusterCost(clusterStart, clusterEnd);
        v[numPoints - 1] = cost;

#pragma omp critical
        {
            std::cout << "DEBUG: v[" << (numPoints - 1) << "] = " << cost
                      << " (cluster [" << clusterStart << ", " << clusterEnd << "], "
                      << numPoints << " points)" << std::endl;
        }
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
    uint clusterSize = end - start + 1;

    // Parallélisation du calcul des médoïdes pour les clusters suffisamment grands
    bool useParallel = (clusterSize > 20);

#pragma omp critical
    {
        std::cout << "    calculateClusterCost [" << start << ", " << end << "]:" << std::endl;
    }

#pragma omp parallel for if(useParallel) reduction(min:minCost) schedule(dynamic)
    for (uint medoid = start; medoid <= end; medoid++) {
        double cost = 0.0;

        // Calcul séquentiel des distances pour chaque médoïde
        for (uint i = start; i <= end; i++) {
            if (i != medoid) {
                double dist = squaredDistance(i, medoid);
                cost += dist;

#pragma omp critical
                {
                    std::cout << "      dist(" << i << ", " << medoid << ") = " << dist << std::endl;
                }
            }
        }

#pragma omp critical
        {
            std::cout << "    medoid " << medoid << ": cost = " << cost << std::endl;
        }

        if (cost < minCost) {
            minCost = cost;
        }
    }

#pragma omp critical
    {
        std::cout << "    --> minCost = " << minCost << std::endl;
    }

    return minCost;
}