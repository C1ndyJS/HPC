#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función para reservar memoria para una matriz cuadrada
int** reservar_matriz(int n) {
    int** matriz = (int**) malloc(n * sizeof(int*));
    if (!matriz) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        matriz[i] = (int*) malloc(n * sizeof(int));
        if (!matriz[i]) {
            perror("Error al asignar memoria");
            exit(EXIT_FAILURE);
        }
    }
    return matriz;
}

// Función para liberar memoria de una matriz
void liberar_matriz(int** matriz, int n) {
    for (int i = 0; i < n; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

// Función para inicializar la matriz con valores aleatorios
void llenar_matriz(int** matriz, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matriz[i][j] = rand() % 10; // Números entre 0 y 9
        }
    }
}

// Función para imprimir una matriz
void imprimir_matriz(int** matriz, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }
}

// Función para multiplicar matrices fila por fila
void multiplicar_matrices_fila(int** A, int** B, int** C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
        }
        for (int k = 0; k < n; k++) {
            for (int j = 0; j < n; j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <tama\xC3\xB1o de la matriz>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "El tama\xC3\xB1o de la matriz debe ser un n\xC3\xBAmero positivo.\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL)); // Inicializar la semilla para números aleatorios

    // Reservar memoria para las matrices
    int** A = reservar_matriz(n);
    int** B = reservar_matriz(n);
    int** C = reservar_matriz(n);

    // Llenar matrices con valores aleatorios
    llenar_matriz(A, n);
    llenar_matriz(B, n);

    // Medir el tiempo de ejecución
    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // Multiplicación de matrices fila por fila
    multiplicar_matrices_fila(A, B, C, n);

    clock_gettime(CLOCK_MONOTONIC, &fin);

    // Calcular tiempo transcurrido en nanosegundos
    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("\nTiempo de ejecuci\xC3\xB3n: %.6f segundos\n", tiempo);

    // Liberar memoria
    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}
