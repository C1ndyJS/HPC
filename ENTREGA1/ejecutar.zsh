#!/bin/zsh

# Función para ejecutar un programa y guardar los resultados en CSV
ejecutar_test() {
    local programa=$1
    local output_file=$2
    typeset -a hilos
    hilos=(${=3})

    local tamanos=(10 100 200 400 800 1600 3200)
    local num_iteraciones=10
    
    # Escribir encabezado en el CSV
    echo "Tamaño de Matriz,Iteración,Hilos,Tiempo (s)" > $output_file

    for tamano in ${tamanos[@]}; do
        for ((iter = 1; iter <= num_iteraciones; iter++)); do
            for h in ${hilos[@]}; do
                if [[ $h -eq 0 ]]; then
                    tiempo=$(./$programa $tamano | grep "Tiempo de ejecución" | awk '{print $4}')
                else
                    tiempo=$(./$programa $tamano $h $num_iteraciones 2>/dev/null | grep "Ejecutado:" | tail -n 1 | awk -F' -> Tiempo: ' '{print $2}')


                fi
                echo "$tamano,$iter,$h,$tiempo" >> $output_file
                echo "Ejecutado: $programa - Tamaño: $tamano - Iter: $iter - Hilos: $h -> Tiempo: $tiempo s"
            done
        done
    done
    echo "Resultados guardados en $output_file"
}

# Menú interactivo
echo "Seleccione el tipo de ejecución:"
echo "1) Secuencial (matrizSecuencial)"
echo "2) Paralelo con CLOCK_MONOTONIC (MatricesHilos)"
echo "3) Paralelo con CLOCK_PROCESS_CPUTIME_ID (MatricesH2)"
echo "Ingrese una opción (1-3):"
read opcion

case $opcion in
    1)
        ejecutar_test "matrizSecuencial" "tiempos_secuencial.csv" "0"
        ;;
    2)
        ejecutar_test "matricesHilos" "tiempos_monotonic.csv" "2 4 8 16 32"
        ;;
    3)  
        ejecutar_test "matricesH2" "tiempos_process.csv" "2 4 8 16 32"
        ;;
    *)
        echo "Opción no válida."
        exit 1
        ;;
esac
