#!/bin/zsh

# Archivo CSV donde se guardarán los resultados
output_file="tiempos_secuencial.csv"

# Escribir encabezado en el CSV
echo "Tamaño de Matriz,Iteración,Tiempo (s)" > $output_file

# Lista de tamaños de matriz
tamanos=(10 100 200 400 800 1600 3200)

# Número de iteraciones por tamaño
num_iteraciones=10

# Ejecutar cada tamaño de matriz 10 veces
for tamano in ${tamanos[@]}; do
    for iter in {1..10}; do
        # Ejecutar el programa y capturar el tiempo de ejecución
        tiempo=$(./matrices $tamano | grep "Tiempo de ejecución" | awk '{print $4}')

        # Guardar en el archivo CSV
        echo "$tamano,$iter,$tiempo" >> $output_file

        echo "Ejecutado tamaño $tamano, iteración $iter -> Tiempo: $tiempo s"
    done
done

echo "Resultados guardados en $output_file"

