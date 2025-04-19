#include "aux.h"
#include <math.h>
#include <stdio.h>
#include <time.h>

void print_matrix(double *A, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%f ", A[i * size + j]);
		}
		printf("\n");
	}
}
void print_vect(double *b, int size) {
	for (int j = 0; j < size; j++) {
		printf("%f ", b[j]);
	}
	printf("\n");
}

// dont use, calculate correctness with residual_error
int check_result(double *true_solution, double *solution, int dim, int verbose) {
	int correct = 1;
	for (int i = 0; i < dim; i++) {
		// test failed
		if (abs(solution[i] - true_solution[i]) > EPSILON) {
			correct = 0;
			if (verbose)
				printf("\t difference larger than epsilon: "
				       "true[%d]=%f, ours[%d]=%f, dif=%f\n",
				       i, true_solution[i], i, solution[i], abs(solution[i] - true_solution[i]));
		}
	}
	return correct;
}
double calculate_residual_error(struct tridiagonal_matrix *matrix, double *solution, double *b, int matrix_size) {
	double Ax = 0.0;
	double residual = 0.0;

	// Calculate Ax
	for (int i = 0; i < matrix_size; i++) {
		Ax = 0.0;
		if (i > 0) {
			Ax += matrix->dl[i] * solution[i - 1];
		}
		Ax += matrix->d[i] * solution[i];
		if (i < matrix_size - 1) {
			Ax += matrix->du[i] * solution[i + 1];
		}
		residual += (Ax - b[i]) * (Ax - b[i]);
	}

	return sqrt(residual / calculate_l2_norm(b, matrix_size));
}
double calculate_l2_norm(double *vector, int size) {
	double norm = 0.0;
	for (int i = 0; i < size; i++) {
		norm += vector[i] * vector[i];
	}
	return (norm);
}

void extract_diagonals(double *A, int size, double *dl, double *d, double *du) {
	for (int i = 0; i < size; i++) {
		d[i] = A[index_of(i, size, i)];
		if (i < size - 1)
			du[i] = A[index_of(i, size, i + 1)];
		if (i > 0)
			dl[i - 1] = A[index_of(i, size, i - 1)];
	}
}
double get_time() {
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec + now.tv_nsec * 1e-9;
}
int read_parallel_results(struct tridiagonal_matrix *A, double *b, double *res, int matrix_size, double *time) {
	char path[32] = {0};
	FILE *f;
	snprintf(path, sizeof(path), "./parallel_solutions/%d", (matrix_size));

	f = fopen(path, "rb");
	if (!f) {
		printf("error opening solution file %s\n", path);
		return 1;
	}
	for (int i = 0; i < matrix_size; i++) {
		A->du[i] = -1.0;
		A->d[i] = 2.0;
		A->dl[i] = -1.0;
	}
	A->dl[0] = 0.0;
	A->du[matrix_size - 1] = 0.0;
	fread(res, sizeof(double), matrix_size, f);
	fread(b, sizeof(double), matrix_size, f);
	fread(time, sizeof(double), 1, f);
	fclose(f);
	return 0;
}
