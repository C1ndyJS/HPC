#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void initialize_matrices(double* A, double* B, int n) {
    srand(time(NULL));
    for (int i = 0; i < n * n; i++) {
        A[i] = (double)rand() / RAND_MAX;
        B[i] = (double)rand() / RAND_MAX;
    }
}

void matrix_multiply_mpi(double* A, double* B, double* C, int n, int rank, int size) {
    int rows_per_process = n / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? n : start_row + rows_per_process;
    
    // Multiplicación local
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < n; j++) {
            C[i*n + j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i*n + j] += A[i*n + k] * B[k*n + j];
            }
        }
    }
}

void print_matrix(double* matrix, int n, const char* name) {
    if (n <= 10) {  // Solo imprime matrices pequeñas
        printf("\nMatrix %s:\n", name);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                printf("%.3f ", matrix[i*n + j]);
            }
            printf("\n");
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: mpirun -np <processes> %s <matrix_size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    int n = atoi(argv[1]);
    
    if (n <= 0) {
        if (rank == 0) {
            printf("Error: Matrix size must be positive\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // Verificar que el tamaño de matriz sea divisible por el número de procesos
    if (n % size != 0) {
        if (rank == 0) {
            printf("Warning: Matrix size %d not evenly divisible by %d processes\n", n, size);
        }
    }
    
    double* A = malloc(n * n * sizeof(double));
    double* B = malloc(n * n * sizeof(double));
    double* C = malloc(n * n * sizeof(double));
    
    if (!A || !B || !C) {
        printf("Error: Memory allocation failed on process %d\n", rank);
        MPI_Finalize();
        return 1;
    }
    
    // Inicialización de matrices (solo proceso 0)
    if (rank == 0) {
        initialize_matrices(A, B, n);
        printf("Starting matrix multiplication: %dx%d with %d processes\n", n, n, size);
    }
    
    // Broadcast de matrices
    MPI_Bcast(A, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Sincronización antes de medir tiempo
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    
    // Realizar multiplicación de matrices
    matrix_multiply_mpi(A, B, C, n, rank, size);
    
    // Recolección de resultados
    int rows_per_process = n / size;
    int remainder = n % size;
    
    // Calcular las cuentas y desplazamientos para cada proceso
    int* sendcounts = malloc(size * sizeof(int));
    int* displs = malloc(size * sizeof(int));
    
    for (int i = 0; i < size; i++) {
        int rows = rows_per_process + (i < remainder ? 1 : 0);
        sendcounts[i] = rows * n;
        displs[i] = (i == 0) ? 0 : displs[i-1] + sendcounts[i-1];
    }
    
    // Usar Allgatherv para manejar distribución no uniforme
    MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                   C, sendcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    
    if (rank == 0) {
        double execution_time = end_time - start_time;
        printf("Matrix size: %d, Processes: %d, Time: %.6f seconds\n", 
               n, size, execution_time);
        
        // Imprimir matrices pequeñas para verificación
        if (n <= 10) {
            print_matrix(A, n, "A");
            print_matrix(B, n, "B");
            print_matrix(C, n, "C = A * B");
        }
    }
    
    free(A);
    free(B);
    free(C);
    free(sendcounts);
    free(displs);
    
    MPI_Finalize();
    return 0;
}