#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

class CSVExporter {
public:
    /**
     * Exporte les résultats en CSV simple
     * Format: point_id,x,y,cluster_id
     */
    static void exportResults(const std::vector<double>& points,
                              const std::vector<size_t>& solution,
                              size_t dimension,
                              const std::string& filename = "results.csv") {

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur: impossible de créer " << filename << std::endl;
            return;
        }

        // Header
        file << "point_id,x,y,cluster_id\n";

        // Data
        size_t numPoints = points.size() / dimension;
        for (size_t i = 0; i < numPoints; i++) {
            file << i << ","
                 << std::fixed << std::setprecision(6)
                 << points[i * dimension] << ","     // x
                 << points[i * dimension + 1] << "," // y
                 << solution[i] << "\n";
        }

        file.close();
        std::cout << "✓ Résultats exportés: " << filename << std::endl;
    }
};
