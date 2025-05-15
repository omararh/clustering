#pragma once

#include <vector>
#include <stdexcept>

/**
 * @class MatrixDouble
 * @brief A 2D matrix class for storing dynamic programming values in clustering
 *
 * This class implements a matrix for storing optimal costs in dynamic programming approach
 * to clustering problems. It provides methods for initialization, element access and manipulation
 * as required by the SolverDP class.
 */
class MatrixDouble {
private:
    std::vector<std::vector<double>> data; // 2D array to store the matrix elements
    size_t rows;                          // Number of rows (k values)
    size_t cols;                          // Number of columns (i values)

public:
    /**
     * @brief Default constructor
     */
    MatrixDouble() : rows(0), cols(0) {}

    /**
     * @brief Destructor
     */
    ~MatrixDouble() {
        // No need for explicit memory management with std::vector
    }

    /**
     * @brief Initialize the matrix with specified dimensions
     * @param numCols Number of columns (corresponds to number of points N)
     * @param numRows Number of rows (corresponds to number of clusters K)
     */
    void initMatrix(size_t numCols, size_t numRows) {
        rows = numRows;
        cols = numCols;

        // Resize and initialize data with zeros
        data.resize(rows);
        for (size_t i = 0; i < rows; ++i) {
            data[i].resize(cols, 0.0);
        }
    }

    /**
     * @brief Get an element from the matrix
     * @param rowIndex Row index (corresponds to cluster index k)
     * @param colIndex Column index (corresponds to point index i)
     * @return Value at the specified position
     */
    double getElement(size_t rowIndex, size_t colIndex) const {
        if (rowIndex >= rows || colIndex >= cols) {
            throw std::out_of_range("Matrix index out of bounds");
        }
        return data[rowIndex][colIndex];
    }

    /**
     * @brief Set an element in the matrix
     * @param rowIndex Row index (corresponds to cluster index k)
     * @param colIndex Column index (corresponds to point index i)
     * @param value Value to set
     */
    void setElement(size_t rowIndex, size_t colIndex, double value) {
        if (rowIndex >= rows || colIndex >= cols) {
            throw std::out_of_range("Matrix index out of bounds");
        }
        data[rowIndex][colIndex] = value;
    }

    /**
     * @brief Delete the matrix and free memory
     */
    void deleteMatrix() {
        for (auto& row : data) {
            row.clear();
        }
        data.clear();
        rows = 0;
        cols = 0;
    }

    /**
     * @brief Get the number of rows
     * @return Number of rows
     */
    size_t getRows() const {
        return rows;
    }

    /**
     * @brief Get the number of columns
     * @return Number of columns
     */
    size_t getCols() const {
        return cols;
    }
};

