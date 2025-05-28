#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <map>
#include <algorithm>
#include "medoidsDP.hpp"
#include "medianDP.hpp"

struct BenchmarkResult {
    std::string instance_name;
    size_t N;
    size_t K;
    double medoids_on_medoids;  // Solution médoïdes évaluée sur critère médoïdes
    double medoids_on_median;   // Solution médoïdes évaluée sur critère médian
    double median_on_medoids;   // Solution médian évaluée sur critère médoïdes
    double median_on_median;    // Solution médian évaluée sur critère médian
};

class BenchmarkSuite {
private:
    std::vector<std::string> instance_files;
    std::vector<size_t> K_values;
    std::vector<BenchmarkResult> results;

    // Évalue une solution sur le critère k-medoids (distances carrées)
    double evaluateOnMedoids(const std::vector<double>& points,
                             const std::vector<size_t>& solution,
                             size_t N, size_t D, size_t K) {
        double total_cost = 0.0;

        for (size_t k = 1; k <= K; k++) {
            std::vector<size_t> cluster_points;
            for (size_t i = 0; i < N; i++) {
                if (solution[i] == k) {
                    cluster_points.push_back(i);
                }
            }

            if (cluster_points.size() <= 1) continue;

            // Trouver le médoïde optimal pour ce cluster
            double min_cost = std::numeric_limits<double>::max();
            for (size_t medoid_idx : cluster_points) {
                double cost = 0.0;
                for (size_t point_idx : cluster_points) {
                    if (point_idx != medoid_idx) {
                        double dist_sq = 0.0;
                        for (size_t d = 0; d < D; d++) {
                            double diff = points[point_idx * D + d] - points[medoid_idx * D + d];
                            dist_sq += diff * diff;
                        }
                        cost += dist_sq;  // Distance carrée pour k-medoids
                    }
                }
                min_cost = std::min(min_cost, cost);
            }
            total_cost += min_cost;
        }

        return total_cost;
    }

    // Évalue une solution sur le critère p-median (distances simples)
    double evaluateOnMedian(const std::vector<double>& points,
                            const std::vector<size_t>& solution,
                            size_t N, size_t D, size_t K) {
        double total_cost = 0.0;

        for (size_t k = 1; k <= K; k++) {
            std::vector<size_t> cluster_points;
            for (size_t i = 0; i < N; i++) {
                if (solution[i] == k) {
                    cluster_points.push_back(i);
                }
            }

            if (cluster_points.size() <= 1) continue;

            // Trouver le médian optimal pour ce cluster
            double min_cost = std::numeric_limits<double>::max();
            for (size_t median_idx : cluster_points) {
                double cost = 0.0;
                for (size_t point_idx : cluster_points) {
                    if (point_idx != median_idx) {
                        double dist_sq = 0.0;
                        for (size_t d = 0; d < D; d++) {
                            double diff = points[point_idx * D + d] - points[median_idx * D + d];
                            dist_sq += diff * diff;
                        }
                        cost += sqrt(dist_sq);  // Distance simple pour p-median
                    }
                }
                min_cost = std::min(min_cost, cost);
            }
            total_cost += min_cost;
        }

        return total_cost;
    }

public:
    BenchmarkSuite(const std::vector<std::string>& instances, const std::vector<size_t>& K_vals)
            : instance_files(instances), K_values(K_vals) {}

    void runBenchmarks() {
        std::cout << "=== BENCHMARK SUITE ===" << std::endl;
        std::cout << "Instances: " << instance_files.size() << std::endl;
        std::cout << "K values: ";
        for (size_t k : K_values) std::cout << k << " ";
        std::cout << std::endl << std::endl;

        for (const std::string& instance_file : instance_files) {
            for (size_t K : K_values) {
                try {
                    std::cout << "Testing: " << instance_file << " with K=" << K << std::endl;

                    // Résoudre avec k-medoids
                    MedoidsDP medoids_solver;
                    medoids_solver.import(instance_file);
                    medoids_solver.setNbClusters(); // Utilise la méthode automatique
                    medoids_solver.solve();

                    // Résoudre avec p-median
                    MedianDP median_solver;
                    median_solver.import(instance_file);
                    median_solver.setNbClusters(); // Utilise la méthode automatique
                    median_solver.solve();

                    // Préparer les données pour évaluation croisée
                    size_t N = medoids_solver.getNbPoints();
                    size_t D = medoids_solver.getDimension();
                    size_t actual_K = medoids_solver.getNbClusters();

                    // Accéder aux données via les méthodes publiques
                    const std::vector<double>& points = medoids_solver.getPoints();
                    const std::vector<size_t>& medoids_solution = medoids_solver.getSolution();
                    const std::vector<size_t>& median_solution = median_solver.getSolution();

                    // Évaluations croisées
                    BenchmarkResult result;
                    result.instance_name = std::filesystem::path(instance_file).stem();
                    result.N = N;
                    result.K = actual_K;

                    result.medoids_on_medoids = evaluateOnMedoids(points, medoids_solution, N, D, actual_K);
                    result.medoids_on_median = evaluateOnMedian(points, medoids_solution, N, D, actual_K);
                    result.median_on_medoids = evaluateOnMedoids(points, median_solution, N, D, actual_K);
                    result.median_on_median = evaluateOnMedian(points, median_solution, N, D, actual_K);

                    results.push_back(result);

                    std::cout << "  ✓ Completed" << std::endl;

                } catch (const std::exception& e) {
                    std::cerr << "  ✗ Error: " << e.what() << std::endl;
                }
            }
        }
    }

    void exportResults(const std::string& output_file = "benchmark_results.csv") {
        std::ofstream file(output_file);
        if (!file.is_open()) {
            std::cerr << "Erreur: impossible de créer " << output_file << std::endl;
            return;
        }

        // Header
        file << "instance,N,K,medoids_on_medoids,medoids_on_median,median_on_medoids,median_on_median,";
        file << "ratio_medoids_median_on_medoids,ratio_median_medoids_on_median" << std::endl;

        // Data avec ratios normalisés
        for (const auto& result : results) {
            double ratio_medoids_median_on_medoids = result.median_on_medoids / result.medoids_on_medoids;
            double ratio_median_medoids_on_median = result.medoids_on_median / result.median_on_median;

            file << result.instance_name << ","
                 << result.N << ","
                 << result.K << ","
                 << std::fixed << std::setprecision(6)
                 << result.medoids_on_medoids << ","
                 << result.medoids_on_median << ","
                 << result.median_on_medoids << ","
                 << result.median_on_median << ","
                 << ratio_medoids_median_on_medoids << ","
                 << ratio_median_medoids_on_median << std::endl;
        }

        file.close();
        std::cout << "✓ Résultats exportés: " << output_file << std::endl;
    }

    void printSummaryByN() {
        // Regrouper par taille N
        std::map<size_t, std::vector<BenchmarkResult*>> by_N;
        for (auto& result : results) {
            by_N[result.N].push_back(&result);
        }

        std::cout << "\n=== RÉSUMÉ PAR TAILLE N ===" << std::endl;
        for (const auto& [N, group] : by_N) {
            double avg_ratio_medoids_median = 0.0;
            double avg_ratio_median_medoids = 0.0;

            for (const auto* result : group) {
                avg_ratio_medoids_median += result->median_on_medoids / result->medoids_on_medoids;
                avg_ratio_median_medoids += result->medoids_on_median / result->median_on_median;
            }

            avg_ratio_medoids_median /= group.size();
            avg_ratio_median_medoids /= group.size();

            std::cout << "N=" << N << " (n=" << group.size() << " instances):" << std::endl;
            std::cout << "  Ratio moyen Median/Medoids sur critère Medoids: "
                      << std::fixed << std::setprecision(3) << avg_ratio_medoids_median << std::endl;
            std::cout << "  Ratio moyen Medoids/Median sur critère Median: "
                      << std::fixed << std::setprecision(3) << avg_ratio_median_medoids << std::endl;
        }
    }
};

int main(int argc, char** argv) {
    std::cout << "===============================================" << std::endl;
    std::cout << "  BENCHMARK CROSS-VALIDATION SOLVERS" << std::endl;
    std::cout << "===============================================" << std::endl;

    // Configuration par défaut
    std::vector<std::string> instances = {
            "data/very_small_instance_3.txt",
            "data/very_small_instance_2.txt",
            "data/very_small_instance_4.csv",
            "data/very_small_instance_1.txt"
    };

    std::vector<size_t> K_values = {2, 3, 4, 5};

    // Parsing arguments simples
    if (argc > 1) {
        std::string data_dir = argv[1];
        instances.clear();

        // Scanner le répertoire pour les fichiers .txt
        try {
            for (const auto& entry : std::filesystem::directory_iterator(data_dir)) {
                if (entry.path().extension() == ".txt") {
                    instances.push_back(entry.path().string());
                }
            }
            std::sort(instances.begin(), instances.end());
        } catch (const std::exception& e) {
            std::cerr << "Erreur lecture répertoire: " << e.what() << std::endl;
            return 1;
        }
    }

    if (argc > 2) {
        // K values depuis arguments
        K_values.clear();
        for (int i = 2; i < argc; i++) {
            K_values.push_back(std::stoul(argv[i]));
        }
    }

    // Lancer les benchmarks
    BenchmarkSuite suite(instances, K_values);
    suite.runBenchmarks();

    // Exporter et analyser
    suite.exportResults("results/benchmark_cross_validation.csv");
    suite.printSummaryByN();

    std::cout << "\n✓ Benchmark terminé. Consultez benchmark_cross_validation.csv" << std::endl;

    return 0;
}