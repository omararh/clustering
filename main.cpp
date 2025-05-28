#include <iostream>
#include <string>
#include "medoidsDP.hpp"

int main(int argc, char** argv) {
    std::string filename = "data/very_small_instance_3.txt";
    if (argc > 1) {
        filename = argv[1];
    }

    std::cout << "===========================================" << std::endl;
    std::cout << "  K-Médoïdes DP" << std::endl;
    std::cout << "===========================================" << std::endl;

    try {
        MedoidsDP solver;

        std::cout << "Importing data from: " << filename << std::endl;
        solver.import(filename);

        solver.setNbClusters();
        std::cout << "Nombre de clusters: " << solver.getNbClusters() << std::endl;

        std::cout << "Résolution en cours..." << std::endl;
        solver.solve();

        std::cout << "Coût de la solution: " << solver.getSolutionCost() << std::endl;

        solver.displaySolution();

        // Vérification
        double realCost = solver.calculateRealClusterCost();
        std::cout << "\nVérification: coût réel = " << realCost << std::endl;
        if (std::abs(solver.getSolutionCost() - realCost) < 0.001) {
            std::cout << "✓ Cohérence vérifiée!" << std::endl;
        } else {
            std::cout << "✗ Incohérence détectée!" << std::endl;
        }

        // Export automatique en CSV
        std::string csvFilename = filename;
        size_t lastSlash = csvFilename.find_last_of('/');
        if (lastSlash != std::string::npos) {
            csvFilename = csvFilename.substr(lastSlash + 1);
        }
        if (csvFilename.find(".txt") != std::string::npos) {
            csvFilename.replace(csvFilename.find(".txt"), 4, ".csv");
        }

        solver.saveToCSV("results/medoids/" + csvFilename);

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}