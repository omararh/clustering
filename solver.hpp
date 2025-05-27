#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include "CSVExporter.hpp"

using namespace std;

class Solver {
protected:
    size_t D; // Dimension
    vector<double> points; // Coordonnées plates
    size_t N; // Nombre de points
    size_t K; // Nombre de clusters
    vector<size_t> solution; // Affectation des clusters
    double solutionCost;
    bool isSorted;

    void displayPoint(size_t index) const {
        std::cout << "( ";
        for (size_t d = 0; d < D; ++d) {
            std::cout << getCoordinate(index, d);
            if (d < D - 1) std::cout << ", ";
        }
        std::cout << " )";
    }

    virtual double squaredDistance(size_t i, size_t j) const {
        double result = 0.0;
        for (size_t d = 0; d < D; ++d) {
            double diff = getCoordinate(i, d) - getCoordinate(j, d);
            result += diff * diff;
        }
        return result;
    }

    inline double getCoordinate(size_t pointIndex, size_t dim) const {
        return points[D * pointIndex + dim];
    }

public:
    Solver() : D(0), K(0), N(0), solutionCost(0.0), isSorted(false) {}
    virtual ~Solver() = default;

    virtual void solve() = 0;

    double getSolutionCost() const { return solutionCost; }
    size_t getNbClusters() const { return K; }
    size_t getNbPoints() const { return N; }
    size_t getDimension() const { return D; }

    void setNbClusters() {
        K = std::max(3u, static_cast<unsigned int>(std::sqrt(N)));
        std::cout << "Nombre de clusters défini à: " << K << std::endl;
    }

    void import(const string& filename);
    void displaySolution() const;

    // Export CSV simple
    void saveToCSV(const std::string& filename = "results.csv") const {
        if (solution.empty()) {
            std::cerr << "Erreur: Aucune solution à exporter. Exécutez solve() d'abord." << std::endl;
            return;
        }
        CSVExporter::exportResults(points, solution, D, filename);
    }
};