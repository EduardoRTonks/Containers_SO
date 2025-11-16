#!/bin/bash
# Script para executar o Experimento 1: Overhead de Monitoramento

set -e

echo "--- Compilando todas as ferramentas... ---"
make all
make test_io # Usaremos o test_io como nossa carga de trabalho

# --- Carga de Trabalho ---
# Vamos modificar o test_io para rodar por um tempo fixo
# (Criando um teste de IO limitado)
echo "Compilando carga de trabalho (io_workload)..."
(
cat <<EOF
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define CHUNK_SIZE (10 * 1024 * 1024)
#define FILE_NAME "temp_io_test.dat"
int main() {
    char *buffer = (char*)malloc(CHUNK_SIZE);
    // 500 iterações (10x mais carga)
    for (int i = 0; i < 500; i++) { 
        FILE *fp = fopen(FILE_NAME, "wb");
        fwrite(buffer, 1, CHUNK_SIZE, fp);
        fclose(fp);
        fp = fopen(FILE_NAME, "rb");
        fread(buffer, 1, CHUNK_SIZE, fp);
        fclose(fp);
        remove(FILE_NAME);
    }
    free(buffer);
    return 0;
}
EOF
) | gcc -x c - -o bin/io_workload

# --------------------------

echo "--- 1. Executando Baseline (sem monitor)... ---"
# 'time -p' imprime o tempo em formato 'real <tempo>'
( time -p ./bin/io_workload ) 2> baseline_time.txt
# Captura o tempo 'real'
TIME_BASELINE=$(grep 'real' baseline_time.txt | awk '{print $2}')
echo "Tempo Baseline: $TIME_BASELINE segundos"
rm baseline_time.txt

echo
echo "--- 2. Executando com Monitor... ---"

# Inicia o monitor em background
./bin/resource_monitor $(pidof io_workload) > /dev/null &
MONITOR_PID=$!

# Executa a carga de trabalho
( time -p ./bin/io_workload ) 2> monitor_time.txt
TIME_MONITORED=$(grep 'real' monitor_time.txt | awk '{print $2}')

# Para o monitor
kill $MONITOR_PID
echo "Tempo Monitorado: $TIME_MONITORED segundos"
rm monitor_time.txt

echo
echo "--- 3. Resultados (Experimento 1) ---"
# Calcula a diferença
OVERHEAD=$(echo "$TIME_MONITORED - $TIME_BASELINE" | bc -l)
echo "Overhead do Monitor: $OVERHEAD segundos"