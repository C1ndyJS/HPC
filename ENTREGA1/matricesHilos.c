/*CLOCK_MONOTONIC mide el tiempo transcurrido desde un punto fijo en el pasado, 
sin verse afectado por ajustes del reloj del sistema (como cambios manuales de hora o sincronización de NTP).
Mide el tiempo real que pasa desde el inicio hasta el final de la ejecución del programa, 
incluyendo posibles pausas o bloqueos del sistema operativo.
Es útil cuando queremos medir el tiempo total de ejecución, sin importar si el proceso estuvo inactivo por momentos. 

Usa CLOCK_PROCESS_CPUTIME_ID en el caso secuencial para medir cuánta CPU realmente se usa.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int inicio, fin, n;
    int** A;
    int** B;
    int** C;
} DatosHilo;

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

// Función que ejecutarán los hilos
void* multiplicar_paralelo(void* arg) {
    DatosHilo* datos = (DatosHilo*) arg;
    for (int i = datos->inicio; i < datos->fin; i++) {
        for (int j = 0; j < datos->n; j++) {
            datos->C[i][j] = 0;
            for (int k = 0; k < datos->n; k++) {
                datos->C[i][j] += datos->A[i][k] * datos->B[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamaño de la matriz> <número de hilos>\n", argv[0]);
        return EXIT_FAILURE;
    }   

    int n = atoi(argv[1]);
    int num_hilos = atoi(argv[2]);

    if (n <= 0 || num_hilos <= 0) {
        fprintf(stderr, "El tamaño de la matriz y el número de hilos deben ser números positivos.\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL));

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

    // Crear hilos
    pthread_t hilos[num_hilos];
    DatosHilo datos[num_hilos];

    int filas_por_hilo = n / num_hilos;
    int filas_extra = n % num_hilos;
    int inicio_fila = 0;

    for (int i = 0; i < num_hilos; i++) {
        int filas_a_asignar = filas_por_hilo + (i < filas_extra ? 1 : 0);
        datos[i].inicio = inicio_fila;
        datos[i].fin = inicio_fila + filas_a_asignar;
        datos[i].n = n;
        datos[i].A = A;
        datos[i].B = B;
        datos[i].C = C;

        pthread_create(&hilos[i], NULL, multiplicar_paralelo, (void*)&datos[i]);

        inicio_fila += filas_a_asignar;
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);

    // Calcular tiempo transcurrido en segundos
    double tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("\nTiempo de ejecución: %.6f segundos\n", tiempo);

    // Liberar memoria
    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}
