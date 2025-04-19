#ifndef _AUX_H
#define _AUX_H

#include "tridiagonal_matrix.h"

#define index_of(...) index_(__VA_ARGS__)
#define index_(i, size, j) ((i) * (size) + (j))
#define abs(x) ((x) < 0 ? -(x) : (x))
#define EPSILON 1e-5
#define GREEN_TEXT(x) "\033[32m" x "\033[0m"
#define RED_TEXT(x) "\033[31m" x "\033[0m"

void print_matrix(double *A, int size);
void print_vect(double *b, int size);
double calculate_l2_norm(double *vector, int size);
int check_result(double *true_solution, double *solution, int dim, int verbose);
double calculate_residual_error(struct tridiagonal_matrix *matrix, double *solution, double *b, int matrix_size);
double calculate_l2_norm(double *vector, int size);
void extract_diagonals(double *A, int size, double *dl, double *d, double *du);
double get_time();
int read_parallel_results(struct tridiagonal_matrix *A, double *b, double *res, int matrix_size, double *time);

#endif // !_AUX_H
