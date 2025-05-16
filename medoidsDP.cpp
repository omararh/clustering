#include "medoidsDP.hpp"
#include <limits>
#include <cmath>

// Calcule le coût pour tous les clusters possibles se terminant à l'indice i
void MedoidsDP::clusterCostsBefore(uint i, vector<double> & v) {
    // Pour chaque longueur possible du cluster finissant à l'indice i
    for(uint len = 0; len < v.size(); len++) {
        // Si la longueur est valide (i-len >= 0)
        if(len <= i) {
            // Déterminer les indices de début et fin du cluster
            uint start = i - len;
            uint end = i;

            // Initialiser le coût minimum avec une valeur infinie
            double minCost = std::numeric_limits<double>::max();

            // Tester chaque point du cluster comme médoïde potentiel
            for(uint medoidIndex = start; medoidIndex <= end; medoidIndex++) {
                // Calculer la somme des distances au carré entre le médoïde et les autres points
                double cost = 0.0;
                for(uint j = start; j <= end; j++) {
                    if(j != medoidIndex) {
                        // Utiliser la méthode squaredDistance de la classe parente
                        cost += squaredDistance(j, medoidIndex);
                    }
                }

                // Mettre à jour le coût minimum si nécessaire
                if(cost < minCost) {
                    minCost = cost;
                }
            }

            // Stocker le coût minimal pour ce cluster de longueur len
            v[len] = minCost;
        } else {
            // Si la longueur dépasse i, mettre une valeur "infinie"
            v[len] = std::numeric_limits<double>::max();
        }
    }
}

// Calcule le coût pour tous les clusters possibles commençant au début
void MedoidsDP::clusterCostsFromBeginning(vector<double> & v) {
    // Pour chaque longueur possible de cluster
    for(uint len = 0; len < v.size(); len++) {
        // Si la longueur est valide
        if(len < N) {
            // Le cluster va de 0 à len
            uint start = 0;
            uint end = len;

            // Initialiser le coût minimum avec une valeur infinie
            double minCost = std::numeric_limits<double>::max();

            // Tester chaque point du cluster comme médoïde potentiel
            for(uint medoidIndex = start; medoidIndex <= end; medoidIndex++) {
                // Calculer la somme des distances au carré entre le médoïde et les autres points
                double cost = 0.0;
                for(uint j = start; j <= end; j++) {
                    if(j != medoidIndex) {
                        // Utiliser la méthode squaredDistance de la classe parente
                        cost += squaredDistance(j, medoidIndex);
                    }
                }

                // Mettre à jour le coût minimum si nécessaire
                if(cost < minCost) {
                    minCost = cost;
                }
            }

            // Stocker le coût minimal pour ce cluster
            v[len] = minCost;
        } else {
            // Si la longueur dépasse N, mettre une valeur "infinie"
            v[len] = std::numeric_limits<double>::max();
        }
    }
}