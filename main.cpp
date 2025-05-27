#include <iostream>
#include <string>
#include "medoidsDP.hpp"

int main(int argc, char** argv) {
    std::string filename = "data/very_small_instance_2.txt";
    if (argc > 1) {
        filename = argv[1];
    }

    std::cout << "===========================================" << std::endl;
    std::cout << "  K-Médoïdes DP (Architecture Restructurée)" << std::endl;
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

        std::cout << "Matrice DP:" << std::endl;
        solver.printMatrixDP();

        std::cout << "Coûts finaux:" << std::endl;
        solver.printFinalCosts(" | ");

        solver.displaySolution();

        // Vérification
        double realCost = solver.calculateRealClusterCost();
        std::cout << "\nVérification: coût réel = " << realCost << std::endl;
        if (std::abs(solver.getSolutionCost() - realCost) < 0.001) {
            std::cout << "✓ Cohérence vérifiée!" << std::endl;
        } else {
            std::cout << "✗ Incohérence détectée!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
