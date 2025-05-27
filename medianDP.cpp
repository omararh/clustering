#include "medianDP.hpp"
#include <limits>
#include <iostream>

void MedianDP::clusterCostsBefore(uint i, vector<double>& v) {
    // Calcule les coûts des clusters se terminant à l'index i pour p-median (α=1)
    // v[j] = coût d'un cluster de j+1 points se terminant à i
    
    std::cout << "DEBUG MedianDP::clusterCostsBefore: i=" << i << " v.size()=" << v.size() << std::endl;
    
    // Réinitialiser le vecteur
    std::fill(v.begin(), v.end(), std::numeric_limits<double>::max());
    
    if (v.size() == 0 || i >= N) return;
    
    // Pour chaque nombre de points possible dans le cluster (1 à i+1)
    for (uint numPoints = 1; numPoints <= i + 1 && numPoints <= v.size(); numPoints++) {
        uint clusterStart = i - numPoints + 1;  // Index de début du cluster
        uint clusterEnd = i;                    // Index de fin du cluster (inclus)
        
        if (clusterStart > clusterEnd || clusterEnd >= N) {
            continue;
        }
        
        double cost = calculateClusterCost(clusterStart, clusterEnd);
        v[numPoints - 1] = cost;  // v[0] = coût pour 1 point, v[1] = coût pour 2 points, etc.
        
        std::cout << "DEBUG: v[" << (numPoints - 1) << "] = " << cost 
                  << " (cluster [" << clusterStart << ", " << clusterEnd << "], " 
                  << numPoints << " points)" << std::endl;
    }
}

void MedianDP::clusterCostsFromBeginning(vector<double>& v) {
    // Calcule les coûts des clusters commençant à l'index 0 pour p-median (α=1)
    // v[j] = coût d'un cluster des j+1 premiers points
    
    std::cout << "DEBUG MedianDP::clusterCostsFromBeginning: v.size()=" << v.size() << std::endl;
    
    // Réinitialiser le vecteur
    std::fill(v.begin(), v.end(), std::numeric_limits<double>::max());
    
    if (v.size() == 0) return;
    
    // Pour chaque nombre de points possible dans le cluster (1 à N)
    for (uint numPoints = 1; numPoints <= N && numPoints <= v.size(); numPoints++) {
        uint clusterStart = 0;                  // Toujours commencer à 0
        uint clusterEnd = numPoints - 1;        // Index de fin (inclus) pour numPoints points
        
        if (clusterEnd >= N) {
            continue;
        }
        
        double cost = calculateClusterCost(clusterStart, clusterEnd);
        v[numPoints - 1] = cost;  // v[0] = coût pour 1 point, v[1] = coût pour 2 points, etc.
        
        std::cout << "DEBUG: v[" << (numPoints - 1) << "] = " << cost 
                  << " (cluster [" << clusterStart << ", " << clusterEnd << "], " 
                  << numPoints << " points)" << std::endl;
    }
}

double MedianDP::calculateClusterCost(uint start, uint end) const {
    if (start > end) return 0.0;
    if (start == end) return 0.0; // Un seul point = coût 0
    
    double minCost = std::numeric_limits<double>::max();
    
    std::cout << "    MedianDP::calculateClusterCost [" << start << ", " << end << "]:" << std::endl;
    
    // Tester chaque point comme médian (α = 1)
    for (uint median = start; median <= end; median++) {
        double cost = 0.0;
        for (uint i = start; i <= end; i++) {
            if (i != median) {
                // Distance euclidienne (pas au carré) pour p-median
                double dist = sqrt(squaredDistance(i, median));
                cost += dist;
                std::cout << "      dist(" << i << ", " << median << ") = " << dist << std::endl;
            }
        }
        std::cout << "    median " << median << ": cost = " << cost << std::endl;
        minCost = std::min(minCost, cost);
    }
    
    std::cout << "    --> minCost = " << minCost << std::endl;
    return minCost;
}
