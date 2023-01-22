#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

using namespace std;

class Matrix{
private:
    vector<double> bills, actual_bills, roots;
    double matrix[4][5];
public:
    explicit Matrix (const string& file_name);
    Matrix& operator=(const Matrix& other);

    void divide_row(int row, double divider);
    void multiply_row(int row, double multiplier);
    void subtract_row_from_another(int row, int subtraction_row, double multiplier);
    void switch_rows(int row, int other_row);
    void check_if_matrix_has_only_1_solution(int given_row);
    void fill_actual_bills();
    void fill_matrix_roots();
    void print_matrix();
    void print_result();
};

#endif
