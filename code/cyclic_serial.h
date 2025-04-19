#ifndef __CYCLIC_H
#define __CYCLIC_H

#include "tridiagonal_matrix.h"

double cyclic_reduction_seq_low_mem(struct tridiagonal_matrix *A, double b[], double *result);
double cyclic_reduction_seq_high_mem(struct tridiagonal_matrix *A, double b[], double *result);
#endif // !__CYCLIC_H
