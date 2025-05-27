#include "medianDP.hpp"
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
void MedianDP::clusterCostsBefore(uint i, vector<double>& v) {
    std::cout << "DEBUG MedianDP::clusterCostsBefore: i=" << i << " v.size()=" << v.size() << std::endl;

    std::fill(v.begin(), v.end(), std::numeric_limits<double>::max());

    if (v.size() == 0 || i >= N) return;

    // Parallélisation de la boucle principale si suffisamment de calculs
    bool useParallel = (i > 50); // Seuil pour éviter l'overhead sur petites instances

    // Calculer la limite supérieure pour la boucle
    uint maxPoints = std::min(static_cast<uint>(i + 1), static_cast<uint>(v.size()));

#pragma omp parallel for if(useParallel) schedule(dynamic)
    for (uint numPoints = 1; numPoints <= maxPoints; numPoints++) {
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
void MedianDP::clusterCostsFromBeginning(vector<double>& v) {
    std::cout << "DEBUG MedianDP::clusterCostsFromBeginning: v.size()=" << v.size() << std::endl;

    std::fill(v.begin(), v.end(), std::numeric_limits<double>::max());

    if (v.size() == 0) return;

    // Parallélisation pour les grandes instances
    bool useParallel = (N > 100);

    // Calculer la limite supérieure pour la boucle
    uint maxPoints = std::min(static_cast<uint>(N), static_cast<uint>(v.size()));

#pragma omp parallel for if(useParallel) schedule(dynamic)
    for (uint numPoints = 1; numPoints <= maxPoints; numPoints++) {
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
 * Calculates the optimal p-median cost for a cluster of consecutive points
 * Tests each point in the range as a potential median and returns the minimum cost
 * Cost is the sum of Euclidean distances (not squared) from all points to the median
 *
 * @param start Starting index of the cluster (inclusive)
 * @param end Ending index of the cluster (inclusive)
 * @return Minimum cost achieved by the optimal median selection
 */
double MedianDP::calculateClusterCost(uint start, uint end) const {
    if (start > end) return 0.0;
    if (start == end) return 0.0;

    double minCost = std::numeric_limits<double>::max();
    uint clusterSize = end - start + 1;

    // Parallélisation du calcul des médians pour les clusters suffisamment grands
    bool useParallel = (clusterSize > 20);

#pragma omp critical
    {
        std::cout << "    MedianDP::calculateClusterCost [" << start << ", " << end << "]:" << std::endl;
    }

#pragma omp parallel for if(useParallel) reduction(min:minCost) schedule(dynamic)
    for (uint median = start; median <= end; median++) {
        double cost = 0.0;

        // Calcul séquentiel des distances pour chaque median
        for (uint i = start; i <= end; i++) {
            if (i != median) {
                double dist = sqrt(squaredDistance(i, median));
                cost += dist;

#pragma omp critical
                {
                    std::cout << "      dist(" << i << ", " << median << ") = " << dist << std::endl;
                }
            }
        }

#pragma omp critical
        {
            std::cout << "    median " << median << ": cost = " << cost << std::endl;
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