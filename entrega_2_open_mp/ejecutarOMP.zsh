#!/bin/zsh

# Función para ejecutar el programa con OpenMP y guardar los resultados en CSV
ejecutar_test_openmp() {
    local programa="matriz_openmp"
    local output_file="tiempos_openmp.csv"
    typeset -a hilos
    hilos=(2 4 8 16 32)

    local tamanos=(10 100 200 400 800 1600 3200)
    local num_iteraciones=10

    # Escribir encabezado en el CSV
    echo "Tamaño de Matriz,Iteración,Hilos,Tiempo (s)" > $output_file

    for tamano in ${tamanos[@]}; do
        for h in ${hilos[@]}; do
            # Ejecutar el binario que imprimirá 10 líneas (una por iteración)
            salida=$(./$programa $tamano $h $num_iteraciones 2>/dev/null)

            # Procesar cada línea con grep y sed
            echo "$salida" | grep "Ejecutado:" | while read linea; do
                iter=$(echo "$linea" | sed -n 's/.*Iter: \([0-9]*\) -.*/\1/p')
                tiempo=$(echo "$linea" | sed -n 's/.*-> Tiempo: \([0-9.]*\).*/\1/p')
                echo "$tamano,$iter,$h,$tiempo" >> $output_file
                echo "$linea"
            done
        done
    done
    echo "Resultados guardados en $output_file"
}

# Ejecutar directamente sin menú
ejecutar_test_openmp
