#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

void forward(double *a, double *b, double *c, double *r, int size_per_process, int nprocs, int mynode);
void forward_det(double *a, double *b, double *c, double *x, double *r, int size_per_process, int nprocs, int mynode);
void backsubstitution(double *a, double *b, double *c, double *x, double *r, int size_per_process, int nprocs, int mynode);
void write_results(double *solution, double *rhs, int matrix_size, double *time);
int main(int argc, char **argv) {
	int nprocs;
	int mynode;
	int size_per_process;
	int matrix_size;
	double *dl, *d, *du, *x, *b;
	double *b_temp;
	double start_time, elapsed_time;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
	if (argc < 2) {
		if (mynode == 0) {
			printf("give matrix size as argument as a power of 2\n");
		}
		MPI_Finalize();
		return EXIT_FAILURE;
	}
	srand(time(NULL));

	matrix_size = atoi(argv[1]);

	size_per_process = matrix_size / nprocs;

	dl = malloc(sizeof(double) * (size_per_process + 2)); // upper diag

	d = malloc(sizeof(double) * (size_per_process + 2)); // main diag

	du = malloc(sizeof(double) * (size_per_process + 2)); // sub diag

	b = malloc(sizeof(double) * (size_per_process + 2));	  // rhs
	b_temp = malloc(sizeof(double) * (size_per_process + 2)); // rhs

	x = malloc(sizeof(double) * (size_per_process + 2)); // result array

	for (int i = 0; i < size_per_process + 2; i++) {
		dl[i] = 0.0;
		d[i] = 1.0;
		du[i] = 0.0;
		x[i] = 0.0;
		b[i] = 0.0;
	}
	for (int i = 1; i < size_per_process + 1; i++) {
		dl[i] = -1.0;
		d[i] = 2.0;
		du[i] = -1.0;
		b[i] = ((double)rand()) / RAND_MAX;
		b_temp[i] = b[i];
	}
	start_time = MPI_Wtime();
	forward(dl, d, du, b, size_per_process, nprocs, mynode);
	forward_det(dl, d, du, x, b, size_per_process, nprocs, mynode);
	backsubstitution(dl, d, du, x, b, size_per_process, nprocs, mynode);
	elapsed_time = MPI_Wtime() - start_time;

	MPI_Barrier(MPI_COMM_WORLD);
	double *solution = NULL;
	double *rhs = NULL;
	if (mynode == 0) {
		solution = malloc(sizeof(double) * matrix_size);
		rhs = malloc(sizeof(double) * matrix_size);
	}
	MPI_Gather(x + 1, size_per_process, MPI_DOUBLE, solution, size_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Gather(b_temp + 1, size_per_process, MPI_DOUBLE, rhs, size_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if (mynode == 0) {
		write_results(solution, rhs, matrix_size, &elapsed_time);
	}

	free(dl);
	free(d);
	free(du);
	free(x);
	free(b);
	free(b_temp);

	MPI_Finalize();

	return 0;
}

void write_results(double *solution, double *rhs, int matrix_size, double *time) {
	struct stat st = {0};

	// Check if directory exists; if not, create it
	if (stat("parallel_solutions", &st) == -1) {
		if (mkdir("parallel_solutions", 0700) != 0) {
			perror("Failed to create directory");
			return;
		}
	}

	char file_path[64] = {0};
	snprintf(file_path, sizeof(file_path), "parallel_solutions/%d", matrix_size);

	FILE *file = fopen(file_path, "w");
	if (!file) {
		perror("Error opening solution file");
		return;
	}
	fwrite(solution, sizeof(double), matrix_size, file);
	fwrite(rhs, sizeof(double), matrix_size, file);
	fwrite(time, sizeof(double), 1, file);

	fclose(file);
	printf("Solution written to %s\n", file_path);
}
void forward(double *dl, double *d, double *du, double *b, int size_per_process, int nprocs, int mynode) {
	int i, level;
	int num_levels = log2(size_per_process);
	int dist = 1;
	double alpha, gamma;
	double send_buf[4], recv_buf[4];

	MPI_Status mpi_status, mpi_status_alt;
	MPI_Request mpi_requests[2];

	for (level = 0; level < num_levels; level++) {
		int start_idx = dist * 2;

		if (mynode < nprocs - 1) {
			MPI_Irecv(recv_buf, 4, MPI_DOUBLE, mynode + 1, 0, MPI_COMM_WORLD, &mpi_requests[0]);
		}
		if (mynode > 0) {
			send_buf[0] = dl[dist];
			send_buf[1] = d[dist];
			send_buf[2] = du[dist];
			send_buf[3] = b[dist];
			MPI_Isend(send_buf, 4, MPI_DOUBLE, mynode - 1, 0, MPI_COMM_WORLD, &mpi_requests[1]);
		}
		if (mynode < nprocs - 1) {
			MPI_Wait(&mpi_requests[0], &mpi_status_alt);
			dl[size_per_process + 1] = recv_buf[0];
			d[size_per_process + 1] = recv_buf[1];
			du[size_per_process + 1] = recv_buf[2];
			b[size_per_process + 1] = recv_buf[3];
		}
		for (i = start_idx; i <= size_per_process; i += dist * 2) {
			int prev_idx = i - dist;
			int next_idx = (i + dist < size_per_process + 1) ? i + dist : size_per_process + 1;

			alpha = -dl[i] / d[prev_idx];
			gamma = -du[i] / d[next_idx];

			d[i] += (alpha * du[prev_idx] + gamma * dl[next_idx]);
			dl[i] = alpha * dl[prev_idx];
			du[i] = gamma * du[next_idx];
			b[i] += (alpha * b[prev_idx] + gamma * b[next_idx]);
		}

		dist *= 2;

		if (mynode > 0) {
			MPI_Wait(&mpi_requests[1], &mpi_status);
		}
	}
}
void forward_det(double *dl, double *d, double *du, double *x, double *b, int size_per_process, int nprocs, int mynode) {
	int level, half_procs;
	int nlevel = log2(nprocs);
	int dist_rank = 1;

	double sbuf[4], rbuf_left[4], rbuf_right[4];
	MPI_Status mpi_status;
	MPI_Request mpi_requests[4];

	half_procs = nprocs / 2;

	for (level = 0; level < nlevel - 1; level++) {
		int mynode_level = mynode / dist_rank;
		int nprocs_level = nprocs / dist_rank;

		sbuf[0] = dl[size_per_process];
		sbuf[1] = d[size_per_process];
		sbuf[2] = du[size_per_process];
		sbuf[3] = b[size_per_process];

		if ((mynode_level + 1) % 2 == 0) {
			if (mynode + dist_rank < nprocs) {
				MPI_Irecv(rbuf_right, 4, MPI_DOUBLE, mynode + dist_rank, 100, MPI_COMM_WORLD, &mpi_requests[0]);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode + dist_rank, 101, MPI_COMM_WORLD, &mpi_requests[1]);
			}
			if (mynode - dist_rank >= 0) {
				MPI_Irecv(rbuf_left, 4, MPI_DOUBLE, mynode - dist_rank, 102, MPI_COMM_WORLD, &mpi_requests[2]);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode - dist_rank, 103, MPI_COMM_WORLD, &mpi_requests[3]);
			}
		} else {
			if (mynode + dist_rank < nprocs) {
				MPI_Irecv(rbuf_right, 4, MPI_DOUBLE, mynode + dist_rank, 103, MPI_COMM_WORLD, &mpi_requests[0]);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode + dist_rank, 102, MPI_COMM_WORLD, &mpi_requests[1]);
			}
			if (mynode - dist_rank >= 0) {
				MPI_Irecv(rbuf_left, 4, MPI_DOUBLE, mynode - dist_rank, 101, MPI_COMM_WORLD, &mpi_requests[2]);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode - dist_rank, 100, MPI_COMM_WORLD, &mpi_requests[3]);
			}
		}

		if (mynode + dist_rank < nprocs) {
			MPI_Wait(&mpi_requests[0], &mpi_status);
			dl[size_per_process + 1] = rbuf_right[0];
			d[size_per_process + 1] = rbuf_right[1];
			du[size_per_process + 1] = rbuf_right[2];
			b[size_per_process + 1] = rbuf_right[3];
			MPI_Wait(&mpi_requests[1], &mpi_status);
		}
		if (mynode - dist_rank >= 0) {
			MPI_Wait(&mpi_requests[2], &mpi_status);
			dl[0] = rbuf_left[0];
			d[0] = rbuf_left[1];
			du[0] = rbuf_left[2];
			b[0] = rbuf_left[3];
			MPI_Wait(&mpi_requests[3], &mpi_status);
		}

		int ip = 0, i = size_per_process, in = i + 1;
		double alpha = (mynode_level == 0) ? 0.0 : -dl[i] / d[ip];
		double gamma = (mynode_level == nprocs_level - 1) ? 0.0 : -du[i] / d[in];

		d[i] += alpha * du[ip] + gamma * dl[in];
		dl[i] = alpha * dl[ip];
		du[i] = gamma * du[in];
		b[i] += alpha * b[ip] + gamma * b[in];

		dist_rank *= 2;
	}

	sbuf[0] = dl[size_per_process];
	sbuf[1] = d[size_per_process];
	sbuf[2] = du[size_per_process];
	sbuf[3] = b[size_per_process];

	if (mynode < half_procs) {
		MPI_Irecv(rbuf_right, 4, MPI_DOUBLE, mynode + half_procs, 200, MPI_COMM_WORLD, &mpi_requests[0]);
		MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode + half_procs, 201, MPI_COMM_WORLD, &mpi_requests[1]);

		MPI_Wait(&mpi_requests[0], &mpi_status);
		dl[size_per_process + 1] = rbuf_right[0];
		d[size_per_process + 1] = rbuf_right[1];
		du[size_per_process + 1] = rbuf_right[2];
		b[size_per_process + 1] = rbuf_right[3];

		int i = size_per_process, in = size_per_process + 1;
		double det = d[i] * d[in] - du[i] * dl[in];
		x[i] = (b[i] * d[in] - b[in] * du[i]) / det;
		x[in] = (b[in] * d[i] - b[i] * dl[in]) / det;

		MPI_Wait(&mpi_requests[1], &mpi_status);

	} else {
		MPI_Irecv(rbuf_left, 4, MPI_DOUBLE, mynode - half_procs, 201, MPI_COMM_WORLD, &mpi_requests[2]);
		MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode - half_procs, 200, MPI_COMM_WORLD, &mpi_requests[3]);

		MPI_Wait(&mpi_requests[2], &mpi_status);
		dl[0] = rbuf_left[0];
		d[0] = rbuf_left[1];
		du[0] = rbuf_left[2];
		b[0] = rbuf_left[3];

		int ip = 0, i = size_per_process;
		double det = d[ip] * d[i] - du[ip] * dl[i];
		x[ip] = (b[ip] * d[i] - b[i] * du[ip]) / det;
		x[i] = (b[i] * d[ip] - b[ip] * dl[i]) / det;

		MPI_Wait(&mpi_requests[3], &mpi_status);
	}
}

void backsubstitution(double *dl, double *d, double *du, double *x, double *b, int size_per_process, int nprocs, int mynode) {
	int i, level;
	int num_levels = log2(size_per_process);
	int gap = size_per_process / 2;

	MPI_Status mpi_status;
	MPI_Request mpi_requests[2];

	if (mynode > 0) {
		MPI_Irecv(x, 1, MPI_DOUBLE, mynode - 1, 100, MPI_COMM_WORLD, &mpi_requests[0]);
	}
	if (mynode < nprocs - 1) {
		MPI_Isend(x + size_per_process, 1, MPI_DOUBLE, mynode + 1, 100, MPI_COMM_WORLD, &mpi_requests[1]);
	}
	if (mynode > 0) {
		MPI_Wait(&mpi_requests[0], &mpi_status);
	}

	for (level = num_levels - 1; level >= 0; level--) {
		int stride = gap * 2;
		for (i = size_per_process - gap; i >= 0; i -= stride) {
			int prev = i - gap;
			int next = i + gap;
			x[i] = b[i] - du[i] * x[next] - dl[i] * x[prev];
			x[i] /= d[i];
		}
		gap /= 2;
	}

	if (mynode < nprocs - 1) {
		MPI_Wait(&mpi_requests[1], &mpi_status);
	}
}
