#include "tridiagonal_matrix.h"
#include "aux.h"
#include <stdlib.h>
#include <string.h>

struct tridiagonal_matrix *init_tmatrix(size_t size) {
	struct tridiagonal_matrix *new = malloc(sizeof(struct tridiagonal_matrix));
	new->size = size;
	return new;
}
int alloc_tmatrix(struct tridiagonal_matrix *m) {
	m->du = calloc((m->size), sizeof(double)); // padded with a zero entry at end
	m->d = calloc((m->size), sizeof(double));
	m->dl = calloc((m->size), sizeof(double)); // padded with a zero entry at first index
	return 0;
}

double get_tmatrix_elem(struct tridiagonal_matrix *m, size_t i, size_t j) {
	if (i == j) {
		return m->d[i];
	} else if (i == j + 1) {
		return m->dl[i - 1];
	} else if (j == i + 1) {
		return m->du[i];
	} else {
		return 0.0;
	}
}

void copy_tmatrix(struct tridiagonal_matrix **dest, struct tridiagonal_matrix *src) {
	size_t size = src->size;
	*dest = malloc(sizeof(struct tridiagonal_matrix));
	(*dest)->size = size;
	alloc_tmatrix(*dest);
	memcpy((*dest)->d, src->d, sizeof(double) * size);
	memcpy((*dest)->du, src->du, sizeof(double) * (size));
	memcpy((*dest)->dl, src->dl, sizeof(double) * (size));
}
void convert_to_nxn(struct tridiagonal_matrix *m, double *A) {
	for (size_t i = 0; i < m->size; i++) {
		if (i > 0)
			A[index_of(i, m->size, i - 1)] = m->dl[i];
		if (i < m->size - 1)
			A[index_of(i, m->size, i + 1)] = m->du[i];
		A[index_of(i, m->size, i)] = m->d[i];
	}
}

void free_tmatrix(struct tridiagonal_matrix *m) {
	free(m->du);
	free(m->d);
	free(m->dl);
	free(m);
}
