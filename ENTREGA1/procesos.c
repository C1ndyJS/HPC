#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>     // Para banderas de archivos, a veces necesario

void llenar_matriz(int* matriz, int n) {
    for (int i = 0; i < n * n; i++) {
        matriz[i] = rand() % 10;
    }
}

void imprimir_matriz(int* matriz, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matriz[i*n + j]);
        }
        printf("\n");
    }
}

void multiplicar_parcial(int* A, int* B, int* C, int n, int inicio_fila, int fin_fila) {
    for (int i = inicio_fila; i < fin_fila; i++) {
        for (int j = 0; j < n; j++) {
            int suma = 0;
            for (int k = 0; k < n; k++) {
                suma += A[i*n + k] * B[k*n + j];
            }
            C[i*n + j] = suma;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <tamaño de matriz> <número de procesos> <número de iteraciones>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int num_procesos = atoi(argv[2]);
    int iteraciones = atoi(argv[3]);
    if (n <= 0 || num_procesos <= 0 || iteraciones <= 0) {
        fprintf(stderr, "Parámetros inválidos.\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    // Crear matrices en memoria compartida
    int* A = mmap(NULL, sizeof(int) * n * n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int* B = mmap(NULL, sizeof(int) * n * n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int* C = mmap(NULL, sizeof(int) * n * n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (A == MAP_FAILED || B == MAP_FAILED || C == MAP_FAILED) {
        perror("Error en mmap");
        exit(EXIT_FAILURE);
    }

    llenar_matriz(A, n);
    llenar_matriz(B, n);

    for (int it = 0; it < iteraciones; it++) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        int filas_por_proceso = n / num_procesos;
        int filas_extra = n % num_procesos;
        int inicio_fila = 0;

        for (int i = 0; i < num_procesos; i++) {
            int filas = filas_por_proceso + (i < filas_extra ? 1 : 0);
            pid_t pid = fork();
            if (pid < 0) {
                perror("Error en fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Código del proceso hijo
                multiplicar_parcial(A, B, C, n, inicio_fila, inicio_fila + filas);
                exit(EXIT_SUCCESS);
            }
            inicio_fila += filas;
        }

        // Código del padre: esperar a todos los hijos
        for (int i = 0; i < num_procesos; i++) {
            wait(NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        double tiempo_total = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Iteración %d - Tiempo total: %.6f segundos\n", it + 1, tiempo_total);

        FILE* archivo = fopen("Tiempos/resultados_process_mmap.csv", "a");
        if (archivo != NULL) {
            fprintf(archivo, "%d,%d,%d,%.6f\n", n, it + 1, num_procesos, tiempo_total);
            fclose(archivo);
        } else {
            perror("Error al abrir archivo CSV");
        }
    }

    munmap(A, sizeof(int) * n * n);
    munmap(B, sizeof(int) * n * n);
    munmap(C, sizeof(int) * n * n);

    return 0;
}