// matricesOpenMP.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int** reservar_matriz(int n) {
    int** matriz = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matriz[i] = malloc(n * sizeof(int));
    }
    return matriz;
}

void liberar_matriz(int** matriz, int n) {
    for (int i = 0; i < n; i++) free(matriz[i]);
    free(matriz);
}

void llenar_matriz(int** matriz, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matriz[i][j] = rand() % 10;
}

void multiplicar_matrices(int** A, int** B, int** C, int n, int num_hilos) {
    #pragma omp parallel for num_threads(num_hilos) collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <tamaño_matriz> <num_hilos> <num_iteraciones>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int num_hilos = atoi(argv[2]);
    int iteraciones = atoi(argv[3]);

    srand(time(NULL));

    int** A = reservar_matriz(n);
    int** B = reservar_matriz(n);
    int** C = reservar_matriz(n);
    llenar_matriz(A, n);
    llenar_matriz(B, n);

    for (int iter = 0; iter < iteraciones; iter++) {
        double inicio = omp_get_wtime();
        multiplicar_matrices(A, B, C, n, num_hilos);
        double fin = omp_get_wtime();
        double tiempo = fin - inicio;

        printf("Ejecutado: matriz_openmp - Tamaño: %d - Iter: %d - Hilos: %d -> Tiempo: %.6f\n",
               n, iter + 1, num_hilos, tiempo);
        // Total FLOPs = 2 * N^3 (1 suma + 1 multiplicación por elemento)
        double flops = 2.0 * n * n * n;
        double mflops = (flops / tiempo) / 1e6;

        printf("Resultado: %.2f MFLOPS\n", mflops);
    }

    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}
