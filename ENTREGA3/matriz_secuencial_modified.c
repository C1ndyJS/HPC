#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void matrix_multiply_sequential(double* A, double* B, double* C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i*n + j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i*n + j] += A[i*n + k] * B[k*n + j];
            }
        }
    }
}

void initialize_matrix(double* matrix, int n, int seed) {
    srand(seed);
    for (int i = 0; i < n*n; i++) {
        matrix[i] = (double)rand() / RAND_MAX;
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
    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    
    if (n <= 0) {
        printf("Error: Matrix size must be positive\n");
        return 1;
    }
    
    double* A = malloc(n * n * sizeof(double));
    double* B = malloc(n * n * sizeof(double));
    double* C = malloc(n * n * sizeof(double));
    
    if (!A || !B || !C) {
        printf("Error: Memory allocation failed\n");
        return 1;
    }
    
    // Inicializar matrices con semillas fijas para reproducibilidad
    initialize_matrix(A, n, 1);
    initialize_matrix(B, n, 2);
    
    printf("Starting sequential matrix multiplication: %dx%d\n", n, n);
    
    clock_t start = clock();
    matrix_multiply_sequential(A, B, C, n);
    clock_t end = clock();
    
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Sequential Time: %.6f seconds\n", time_spent);
    
    // Imprimir resultado solo para matrices pequeñas
    if (n <= 10) {
        print_matrix(A, n, "A");
        print_matrix(B, n, "B");
        print_matrix(C, n, "C = A * B");
    }
    
    // Verificar algunos elementos para debugging
    if (n >= 100) {
        printf("Sample results: C[0][0] = %.6f, C[%d][%d] = %.6f\n", 
               C[0], n-1, n-1, C[(n-1)*n + (n-1)]);
    }
    
    free(A);
    free(B);
    free(C);
    
    return 0;
}