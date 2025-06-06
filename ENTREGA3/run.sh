#!/bin/bash

# run_experiments.sh - Script para ejecutar experimentos de multiplicación de matrices
# Autor: Cindy Marcela Jimenez Saldarriaga
# Fecha: Mayo-Junio 2025

# Configuración de parámetros
SIZES=(10 100 200 400 800 1600 3200)
PROCESSES=(2 4 8 16 32)
HOSTFILE="hosts.txt"
RUNS=3  # Número de ejecuciones por configuración para promediar

# Archivos de salida
RESULTS_CSV="results.csv"
LOG_FILE="experiment.log"

# Crear directorio de resultados si no existe
mkdir -p results

# Función para logging
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a $LOG_FILE
}

# Función para verificar que los ejecutables existen
check_executables() {
    if [ ! -f "./matrix_sequential" ]; then
        log "ERROR: matrix_sequential executable not found"
        exit 1
    fi
    
    if [ ! -f "./matrix_mpi" ]; then
        log "ERROR: matrix_mpi executable not found"
        exit 1
    fi
    
    if [ ! -f "$HOSTFILE" ]; then
        log "ERROR: hostfile $HOSTFILE not found"
        exit 1
    fi
}

# Función para compilar los programas
compile_programs() {
    log "Compiling programs..."
    
    # Compilar versión secuencial
    gcc -O3 -o matrix_sequential matrix_sequential.c -lm
    if [ $? -ne 0 ]; then
        log "ERROR: Failed to compile sequential version"
        exit 1
    fi
    
    # Compilar versión MPI
    mpicc -O3 -o matrix_mpi matrix_mpi.c -lm
    if [ $? -ne 0 ]; then
        log "ERROR: Failed to compile MPI version"
        exit 1
    fi
    
    log "Compilation successful"
}

# Función para ejecutar versión secuencial
run_sequential() {
    local size=$1
    log "Running sequential version for matrix size $size"
    
    local total_time=0
    for ((i=1; i<=RUNS; i++)); do
        local output=$(./matrix_sequential $size 2>&1)
        local time=$(echo "$output" | grep "Sequential Time:" | awk '{print $3}')
        
        if [ -z "$time" ]; then
            log "ERROR: Could not extract time from sequential run"
            echo "0"
            return
        fi
        
        total_time=$(echo "$total_time + $time" | bc -l)
    done
    
    local avg_time=$(echo "scale=6; $total_time / $RUNS" | bc -l)
    echo $avg_time
}

# Función para ejecutar versión MPI
run_mpi() {
    local size=$1
    local proc=$2
    
    log "Running MPI version: Size=$size, Processes=$proc"
    
    local total_time=0
    local successful_runs=0
    
    for ((i=1; i<=RUNS; i++)); do
        local output=$(mpirun -np $proc -hostfile $HOSTFILE ./matrix_mpi $size 2>&1)
        local time=$(echo "$output" | grep "Time:" | tail -1 | awk '{print $NF}')
        
        if [ ! -z "$time" ] && [ "$time" != "0.000000" ]; then
            total_time=$(echo "$total_time + $time" | bc -l)
            ((successful_runs++))
        else
            log "WARNING: Failed run $i for Size=$size, Processes=$proc"
        fi
    done
    
    if [ $successful_runs -eq 0 ]; then
        log "ERROR: All runs failed for Size=$size, Processes=$proc"
        echo "0"
        return
    fi
    
    local avg_time=$(echo "scale=6; $total_time / $successful_runs" | bc -l)
    echo $avg_time
}

# Función principal de experimentación
run_experiments() {
    log "Starting matrix multiplication experiments"
    
    # Crear archivo CSV con headers
    echo "Matrix_Size,Processes,Execution_Time,Speedup,Efficiency,Sequential_Time" > $RESULTS_CSV
    
    for size in "${SIZES[@]}"; do
        log "Processing matrix size: $size"
        
        # Ejecutar versión secuencial para baseline
        seq_time=$(run_sequential $size)
        
        if [ "$seq_time" == "0" ]; then
            log "ERROR: Sequential execution failed for size $size"
            continue
        fi
        
        log "Sequential time for size $size: $seq_time seconds"
        
        # Ejecutar versiones paralelas
        for proc in "${PROCESSES[@]}"; do
            # Verificar que tenemos suficientes recursos
            total_slots=$(grep -o 'slots=[0-9]*' $HOSTFILE | cut -d'=' -f2 | paste -sd+ | bc)
            
            if [ $proc -gt $total_slots ]; then
                log "WARNING: Requested $proc processes but only $total_slots slots available"
            fi
            
            mpi_time=$(run_mpi $size $proc)
            
            if [ "$mpi_time" == "0" ]; then
                log "ERROR: MPI execution failed for Size=$size, Processes=$proc"
                continue
            fi
            
            # Calcular métricas
            speedup=$(echo "scale=6; $seq_time / $mpi_time" | bc -l)
            efficiency=$(echo "scale=6; $speedup / $proc" | bc -l)
            
            # Guardar resultados
            echo "$size,$proc,$mpi_time,$speedup,$efficiency,$seq_time" >> $RESULTS_CSV
            
            log "Results: Size=$size, Proc=$proc, Time=$mpi_time, Speedup=$speedup, Efficiency=$efficiency"
        done
    done
    
    log "Experiments completed. Results saved to $RESULTS_CSV"
}

# Función para generar reporte
generate_report() {
    log "Generating performance report"
    
    cat << EOF > performance_report.txt
===============================================
Matrix Multiplication Performance Report
===============================================
Generated: $(date)
Host Configuration: $(cat $HOSTFILE)

Summary of Results:
EOF

    # Encontrar mejores configuraciones
    echo "Best Speedup Configurations:" >> performance_report.txt
    sort -t',' -k4 -nr $RESULTS_CSV | head -5 | while IFS=',' read size proc time speedup eff seq_time; do
        if [ "$size" != "Matrix_Size" ]; then
            echo "  Size: ${size}x${size}, Processes: $proc, Speedup: $speedup" >> performance_report.txt
        fi
    done
    
    echo "" >> performance_report.txt
    echo "Best Efficiency Configurations:" >> performance_report.txt
    sort -t',' -k5 -nr $RESULTS_CSV | head -5 | while IFS=',' read size proc time speedup eff seq_time; do
        if [ "$size" != "Matrix_Size" ]; then
            echo "  Size: ${size}x${size}, Processes: $proc, Efficiency: $eff" >> performance_report.txt
        fi
    done
    
    log "Report generated: performance_report.txt"
}

# Función para limpiar archivos temporales
cleanup() {
    log "Cleaning up temporary files"
    # Opcional: eliminar archivos temporales si los hay
}

# Función para mostrar ayuda
show_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Options:
    -h, --help      Show this help message
    -c, --compile   Compile programs before running
    -s, --sizes     Specify matrix sizes (default: 10 100 200 400 800 1600 3200)
    -p, --processes Specify process counts (default: 2 4 8 16 32)
    -r, --runs      Number of runs per configuration (default: 3)

Examples:
    $0                          # Run with default parameters
    $0 -c                       # Compile and run
    $0 -s "100 400 800" -p "2 4 8"  # Custom sizes and processes
EOF
}

# Parsear argumentos de línea de comandos
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -c|--compile)
            compile_programs
            shift
            ;;
        -s|--sizes)
            IFS=' ' read -ra SIZES <<< "$2"
            shift 2
            ;;
        -p|--processes)
            IFS=' ' read -ra PROCESSES <<< "$2"
            shift 2
            ;;
        -r|--runs)
            RUNS="$2"
            shift 2
            ;;
        *)
            log "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Función principal
main() {
    log "Starting matrix multiplication performance evaluation"
    log "Configuration: Sizes=(${SIZES[*]}), Processes=(${PROCESSES[*]}), Runs=$RUNS"
    
    # Verificar prerrequisitos
    check_executables
    
    # Ejecutar experimentos
    run_experiments
    
    # Generar reporte
    generate_report
    
    # Limpiar
    cleanup
    
    log "All experiments completed successfully!"
    echo "Results available in:"
    echo "  - CSV data: $RESULTS_CSV"
    echo "  - Performance report: performance_report.txt"
    echo "  - Execution log: $LOG_FILE"
}

# Manejar señales para limpieza
trap cleanup EXIT INT TERM

# Ejecutar función principal
main