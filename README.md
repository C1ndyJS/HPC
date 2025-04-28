# HPC
HPC High Performance Computing ISC/UTP 2025/1


# Multiplicación de Matrices en Paralelo con pthreads
Este programa realiza la multiplicación de matrices cuadradas usando programación paralela con hilos (`pthreads`).

## 📦 Compilación

```bash
gcc -o matricesH2 programa.c -pthread

### 🚀 Ejecución
```bash
./matricesH2 <tamaño_matriz> <número_hilos> <número_iteraciones>

### Ejemplo: Multiplica matrices de 500x500, usando 4 hilos y repitiendo el proceso 3 veces.
 ./matricesH2 500 4 3


