#include <iostream>
#include <fstream>
#include <string>
#include "solver.hpp"

// Classe concrète minimale héritant de Solver pour pouvoir l'instancier
class MinimalSolver : public Solver {
public:
    void solve() override {
        std::cout << "MinimalSolver::solve() - Cette méthode n'est pas implémentée." << std::endl;
    }

    using Solver::verifyParetoFront;
//    using Solver::sortPointsByFirstDimension;
};

int main(int argc, char** argv) {
    std::string filename = "data/small_instance.txt";  // Default filename
    if (argc > 1) {
        filename = argv[1];
    }

    std::cout << "===========================================" << std::endl;
    std::cout << "  Front de Pareto Validation Program" << std::endl;
    std::cout << "===========================================" << std::endl;

    try {
        // Vérification manuelle du fichier avant d'utiliser le Solver
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: Unable to open file " << filename << std::endl;
            return 1;
        }

        // Lire les premières lignes du fichier pour diagnostiquer
        std::cout << "Examinons directement le contenu du fichier:" << std::endl;
        double n, d;
        file >> n >> d;
        std::cout << "Valeurs lues: N = " << n << ", D = " << d << std::endl;

        // Compter manuellement le nombre de valeurs dans le fichier
        double value;
        int count = 0;
        while (file >> value) {
            count++;
            if (count <= 10) {
                std::cout << "Valeur " << count << ": " << value << std::endl;
            }
            else if (count == 11) {
                std::cout << "..." << std::endl;
            }
        }
        std::cout << "Nombre total de valeurs dans le fichier: " << count << std::endl;
        std::cout << "Nombre attendu: " << static_cast<int>(n) * static_cast<int>(d) << std::endl;
        file.close();

        std::cout << "\nMaintenant, essayons d'utiliser la classe Solver:" << std::endl;

        // Créer une instance de notre solveur minimal
        MinimalSolver solver;

        // Import the data file
        std::cout << "Importing data from: " << filename << std::endl;
        solver.import(filename);

        // Le reste du code...

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}