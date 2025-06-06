# 🧪 Benchmark OpenMP: Multiplicación de Matrices

Este proyecto corresponde a un laboratorio investigativo enfocado en la evaluación del rendimiento de un algoritmo de multiplicación de matrices cuadradas, utilizando **programación paralela con OpenMP** y herramientas de benchmarking como **Phoronix Test Suite** y **Himeno Benchmark**.

## 📌 Objetivos

- Implementar un algoritmo clásico de multiplicación de matrices con paralelización usando OpenMP.
- Automatizar las pruebas con diferentes tamaños de matriz y números de hilos.
- Medir tiempos de ejecución y rendimiento en MFLOPS.
- Comparar resultados entre dos máquinas con diferente capacidad de hardware.
- Utilizar benchmarks profesionales (Himeno) para validar resultados.
- Construir un perfil de test personalizado en Phoronix Test Suite.

## 🛠️ Herramientas utilizadas

- Lenguaje: **C con OpenMP**
- Benchmark automation: **Phoronix Test Suite**
- Scripts de prueba: **Zsh**
- Visualización: **Excel / Python / Phoronix PDF**
- Benchmarks comparativos: **Himeno Benchmark**

## ⚙️ Estructura del proyecto

```
.
├── matriz_openmp.c                # Código fuente principal (OpenMP)
├── ejecutarOMP.zsh                # Script para pruebas automatizadas por consola
├── resultados/                    # CSVs generados con tiempo y rendimiento
├── openmp-matrix-bench/          # Perfil de test para Phoronix
│── openmp-matrix-bench.xml
│── openmp-matrix-bench.sh
│── openmp-matrix-bench.install
│── openmp-matrix-bench.json
│── matriz_openmp.c
├── graficas/                      # Imágenes o scripts para generar gráficos
├── documentos/                    # Informe final, PDF generado por Phoronix
│   └── informe_laboratorio.pdf
└── README.md                      # Este archivo
```

## 🧪 Cómo ejecutar las pruebas

### 1. Ejecutar desde script `.zsh`:

```bash
chmod +x ejecutarOMP.zsh
./ejecutarOMP.zsh
```

Esto generará un CSV con los tiempos por tamaño de matriz e hilos.

### 2. Correr benchmark personalizado en Phoronix:

```bash
mkdir -p ~/.phoronix-test-suite/pts/
cp -r openmp-matrix-bench ~/.phoronix-test-suite/pts/

phoronix-test-suite user-local-install openmp-matrix-bench
phoronix-test-suite run openmp-matrix-bench
```

### 3. Visualizar los resultados:

- Abrir el CSV con Excel o usar scripts en Python para graficar.
- Exportar resultados a PDF desde Phoronix:

```bash
phoronix-test-suite result-file-to-pdf nombre_resultado
```

## 📊 Gráficos sugeridos

- Tiempo vs Número de Hilos
- MFLOPS vs Hilos
- Tiempo vs Tamaño de Matriz
- Comparación con Himeno Benchmark

## 📘 Informe de laboratorio

Incluye:

- Introducción al problema
- Fundamentos de OpenMP y HPC
- Comparativa entre equipos
- Gráficos y análisis de rendimiento
- Conclusiones y recomendaciones
