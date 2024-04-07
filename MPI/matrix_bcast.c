#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_SIZE 3

// Function to initialize matrices A and B with fixed values
void initialize_matrices(int A[MATRIX_SIZE][MATRIX_SIZE], int B[MATRIX_SIZE][MATRIX_SIZE]) {
    int i, j;
    
    int values_A[MATRIX_SIZE][MATRIX_SIZE] = {
        {0, 1, 2},
        {1, 2, 3},
        {2, 3, 4}
    };

    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = values_A[i][j];
        }
    }

    int values_B[MATRIX_SIZE][MATRIX_SIZE] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            B[i][j] = values_B[i][j];
        }
    }
}

void print_matrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    int i, j;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size, i, j, k;
    int A[MATRIX_SIZE][MATRIX_SIZE], B[MATRIX_SIZE][MATRIX_SIZE], C[MATRIX_SIZE][MATRIX_SIZE];
    int row_A[MATRIX_SIZE], row_C[MATRIX_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Master process initializes matrices A and B
    if (rank == 0) {
        initialize_matrices(A, B);
        printf("Matrix A:\n");
        print_matrix(A);
        printf("\nMatrix B:\n");
        print_matrix(B);
    }

    // Broadcast matrix B to all processes
    MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Scatter rows of matrix A to all processes
    MPI_Scatter(A, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, row_A, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform local multiplication
    for (i = 0; i < MATRIX_SIZE; i++) {
        row_C[i] = 0;
        for (j = 0; j < MATRIX_SIZE; j++) {
            row_C[i] += row_A[j] * B[j][i];
        }
    }

    // Gather results from all processes
    MPI_Gather(row_C, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, C, MATRIX_SIZE * MATRIX_SIZE / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Master process prints the result
    if (rank == 0) {
        printf("\nMatrix C (Result):\n");
        print_matrix(C);
    }

    MPI_Finalize();
    return 0;
}
