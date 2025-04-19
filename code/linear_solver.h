#ifndef __LINEAR_SOLVER_H
#define __LINEAR_SOLVER_H

#include "tridiagonal_matrix.h"

double linear_system_solver(struct tridiagonal_matrix *m, double b[], double *result);
double tridiag_system_solver(struct tridiagonal_matrix *m, double b[], double *result);
#endif // !__LINEAR_SOLVER
