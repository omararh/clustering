#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <functional>

using namespace std;

/**
* @class Solver
* @brief Abstract base class for clustering algorithms
*
* This class provides the foundation for different clustering implementations
* with a focus on front of Pareto optimization problems.
*/
class Solver {
private:
    size_t D; // Dimension of points

    /**
     * @brief Display a single point
     * @param index Index of the point to display
     */
    void displayPoint(size_t index) const;

protected:
    vector<double> points;        // Point coordinates (flat array: x1,y1,x2,y2,...)
    // TODO: add heuristic to return a well chosen 'k'
    size_t K;                     // Number of clusters
    size_t N;                     // Number of points
    vector<size_t> solution;      // Cluster assignment for each point
    double solutionCost;          // Total cost of the current solution
    bool isSorted;                // Flag indicating if points are sorted (important for DP approaches)

    /**
     * @brief Calculate squared Euclidean distance between two points
     * @param i Index of first point
     * @param j Index of second point
     * @return Squared distance
     */
    virtual double squaredDistance(size_t i, size_t j) const;

    /**
     * @brief Calculate Euclidean distance between two points
     * @param i Index of first point
     * @param j Index of second point
     * @return Euclidean distance
     */
    double distance(size_t i, size_t j) const;

    /**
     * @brief Calculate distance with arbitrary power
     * @param i Index of first point
     * @param j Index of second point
     * @param power Power to raise the distance to
     * @return Distance raised to given power
     */
    double distancePower(size_t i, size_t j, double power) const;

    /**
     * @brief Get coordinate of point
     * @param pointIndex Index of the point
     * @param dim Dimension index (0 for x, 1 for y, etc.)
     * @return Coordinate value
     */
    inline double getCoordinate(size_t pointIndex, size_t dim) const {
        return points[D * pointIndex + dim];
    }

    /**
     * @brief Verify if the points form a valid Pareto front (non-dominated solutions)
     * @return True if valid Pareto front, false otherwise
     */
    bool verifyParetoFront() const;

    /**
     * @brief Sort points according to first dimension (useful for DP approaches)
     */
    void sortPointsByFirstDimension();

public:
    /**
     * @brief Constructor
     */
    Solver() : D(0), K(0), N(0), solutionCost(0.0), isSorted(false) {}

    /**
     * @brief Virtual destructor
     */
    virtual ~Solver() = default;

    /**
     * @brief Solve the clustering problem
     * This is a pure virtual function to be implemented by derived classes
     */
    virtual void solve() = 0;

    /**
     * @brief Get the total cost of the current solution
     * @return Solution cost
     */
    double getSolutionCost() const { return solutionCost; }

    /**
     * @brief Get the number of clusters
     * @return Number of clusters
     */
    size_t getNbClusters() const { return K; }

    /**
     * @brief Get the number of points
     * @return Number of points
     */
    size_t getNbPoints() const { return N; }

    /**
     * @brief Get the dimension of points
     * @return Dimension
     */
    size_t getDimension() const { return D; }

    /**
     * @brief Set the number of clusters
     * @param k Number of clusters
     */
    void setNbClusters(size_t k) { K = k; }

    /**
     * @brief Import data from file
     * @param filename Path to data file
     */
    void import(const string& filename);

    /**
     * @brief Display the problem instance
     */
    void displayInstance() const;

    /**
     * @brief Display the current solution
     */
    void displaySolution() const;

    /**
     * @brief Export solution to file
     * @param filename Path to output file
     */
    void exportSolution(const string& filename) const;
};

/**
* @class Solver2D
* @brief Specialized solver for 2D points
*
* Provides optimized implementations for 2D point operations
*/
class Solver2D : public Solver {
protected:
    /**
     * @brief Optimized squared distance calculation for 2D points
     * @param i Index of first point
     * @param j Index of second point
     * @return Squared distance
     */
    double squaredDistance(size_t i, size_t j) const override;
};