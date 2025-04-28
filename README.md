# HPC
HPC High Performance Computing ISC/UTP 2025/1

# Multiplicación de Matrices en Paralelo con pthreads
Este programa realiza la multiplicación de matrices cuadradas usando programación paralela con hilos (`pthreads`).

🧩 ¿Qué hace este programa?
Este programa realiza la multiplicación de dos matrices cuadradas grandes, de manera paralela utilizando varios hilos (threads) para aprovechar mejor el procesador y hacerlo más rápido.

🧠 ¿Cómo funciona en términos generales?
Genera dos matrices (A y B) con números aleatorios.
Crea una cantidad de hilos definida por el usuario.
Cada hilo se encarga de calcular una parte del resultado final (matriz C).
Mide cuánto tiempo tarda cada hilo en hacer su parte.
Repite la operación varias veces (según el número de iteraciones).
Guarda los resultados en un archivo CSV con los tiempos de ejecución, para luego poder analizarlos.

🧪 ¿Para qué sirve?
Este programa sirve para:
Evaluar el rendimiento de la multiplicación de matrices en paralelo.
Ver cómo el número de hilos afecta el tiempo de ejecución.

Obtener datos de rendimiento para comparar (por ejemplo, con otros algoritmos o configuraciones).

## 📦 Compilación

```bash
gcc -o matricesH2 programa.c -pthread

### 🚀 Ejecución
```bash
./matricesH2 <tamaño_matriz> <número_hilos> <número_iteraciones>

### Ejemplo: Multiplica matrices de 500x500, usando 4 hilos y repitiendo el proceso 3 veces.
 ./matricesH2 500 4 3


