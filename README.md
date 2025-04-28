# HPC
HPC High Performance Computing ISC/UTP 2025/1

# Multiplicación de Matrices en Paralelo con pthreads
Este programa realiza la multiplicación de matrices cuadradas usando programación paralela con hilos (`pthreads`).

## 🧩 ¿Qué hace este programa?
Este programa realiza la multiplicación de dos matrices cuadradas grandes, de manera paralela utilizando varios hilos (threads) para aprovechar mejor el procesador y hacerlo más rápido.

## 🧠 ¿Cómo funciona en términos generales?
1. Genera dos matrices (A y B) con números aleatorios.
2. Crea una cantidad de hilos definida por el usuario.
3. Cada hilo se encarga de calcular una parte del resultado final (matriz C).
4. Mide cuánto tiempo tarda cada hilo en hacer su parte.
5. Repite la operación varias veces (según el número de iteraciones).
6. Guarda los resultados en un archivo CSV con los tiempos de ejecución, para luego poder analizarlos.

## 🧪 ¿Para qué sirve?
Este programa sirve para:
- Evaluar el rendimiento de la multiplicación de matrices en paralelo.
- Ver cómo el número de hilos afecta el tiempo de ejecución.

Obtener datos de rendimiento para comparar (por ejemplo, con otros algoritmos o configuraciones).

## 📦 Compilación

```bash
gcc -o matricesH2 programa.c -pthread
```
## 🚀 Ejecución
```bash
./matricesH2 <tamaño_matriz> <número_hilos> <número_iteraciones>
```

## Ejemplo: Multiplica matrices de 500x500, usando 4 hilos y repitiendo el proceso 3 veces.
./matricesH2 500 4 3
Supongamos que quieres saber si usar 2, 4 u 8 hilos hace más rápida la multiplicación de matrices de 1000x1000. Puedes correr el programa así:
```
 ./matricesH2 1000 2 3
 ./matricesH2 1000 4 3
 ./matricesH2 1000 8 3
```

