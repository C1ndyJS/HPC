#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int inicio, fin, n;
    int** A;
    int** B;
    int** C;
    double tiempo;
} DatosHilo;

int** reservar_matriz(int n) {
    int** matriz = (int**) malloc(n * sizeof(int*));
    if (!matriz) { perror("Error al asignar memoria"); exit(EXIT_FAILURE); }
    for (int i = 0; i < n; i++) {
        matriz[i] = (int*) malloc(n * sizeof(int));
        if (!matriz[i]) { perror("Error al asignar memoria"); exit(EXIT_FAILURE); }
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

void* multiplicar_paralelo(void* arg) {
    DatosHilo* datos = (DatosHilo*) arg;
    struct timespec inicio, fin;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &inicio);

    for (int i = datos->inicio; i < datos->fin; i++)
        for (int j = 0; j < datos->n; j++) {
            datos->C[i][j] = 0;
            for (int k = 0; k < datos->n; k++)
                datos->C[i][j] += datos->A[i][k] * datos->B[k][j];
        }
    
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &fin);
    datos->tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <tama\u00f1o de la matriz> <n\u00famero de hilos> <n\u00famero de iteraciones>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]), num_hilos = atoi(argv[2]), iteraciones = atoi(argv[3]);
    if (n <= 0 || num_hilos <= 0 || iteraciones <= 0) {
        fprintf(stderr, "Tama\u00f1o de matriz, n\u00famero de hilos e iteraciones deben ser positivos.\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    int** A = reservar_matriz(n);
    int** B = reservar_matriz(n);
    int** C = reservar_matriz(n);
    llenar_matriz(A, n);
    llenar_matriz(B, n);

    pthread_t hilos[num_hilos];
    DatosHilo datos[num_hilos];

    int filas_por_hilo = n / num_hilos, filas_extra = n % num_hilos, inicio_fila = 0;

    for (int i = 0; i < num_hilos; i++) {
        int filas_a_asignar = filas_por_hilo + (i < filas_extra ? 1 : 0);
        datos[i] = (DatosHilo) {inicio_fila, inicio_fila + filas_a_asignar, n, A, B, C, 0.0};
        inicio_fila += filas_a_asignar;
    }

    for (int it = 0; it < iteraciones; it++) {
        double tiempo_total = 0.0;

        for (int i = 0; i < num_hilos; i++) {
            pthread_create(&hilos[i], NULL, multiplicar_paralelo, (void*)&datos[i]);
        }
        for (int i = 0; i < num_hilos; i++) {
            pthread_join(hilos[i], NULL);
            tiempo_total += datos[i].tiempo;
        }

        double tiempo_promedio = tiempo_total / num_hilos;
        printf("Ejecutado: matricesH2 - Tamaño: %d - Iter: %d - Hilos: %d -> Tiempo: %.6f\n", n, it + 1, num_hilos, tiempo_promedio);

        FILE* archivo = fopen("resultados.csv", "a");
        if (archivo != NULL) {
            fprintf(archivo, "%d,%d,%d,%.6f\n", n, it + 1, num_hilos, tiempo_promedio);
            fclose(archivo);
        } else {
            perror("Error al abrir el archivo CSV");
        }
    }

    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);
    return EXIT_SUCCESS;
}
