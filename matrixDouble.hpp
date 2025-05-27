#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

class MatrixDouble {
private:
    std::vector<std::vector<double>> data;
    size_t rows;
    size_t cols;

public:
    MatrixDouble() : rows(0), cols(0) {}

    void initMatrix(size_t numRows, size_t numCols) {
        rows = numRows;
        cols = numCols;
        data.clear();
        data.resize(rows);
        for (size_t i = 0; i < rows; ++i) {
            data[i].resize(cols, 0.0);
        }
    }

    double getElement(size_t rowIndex, size_t colIndex) const {
        if (rowIndex >= rows || colIndex >= cols) {
            throw std::out_of_range("Matrix index out of bounds");
        }
        return data[rowIndex][colIndex];
    }

    void setElement(size_t rowIndex, size_t colIndex, double value) {
        if (rowIndex >= rows || colIndex >= cols) {
            throw std::out_of_range("Matrix index out of bounds");
        }
        data[rowIndex][colIndex] = value;
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    void deleteMatrix() {
        data.clear();
        rows = 0;
        cols = 0;
    }
};