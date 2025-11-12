#include "monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

IoMetrics get_io_metrics(pid_t pid) {
    char path[256];
    // Cria o caminho para o arquivo /proc/[pid]/io
    snprintf(path, sizeof(path), "/proc/%d/io", pid);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        // Trata o erro (processo inexistente ou sem permissão)
        // Imprime o erro, mas só uma vez se o main estiver em loop
        // perror("Erro ao abrir /proc/[pid]/io"); 
        return (IoMetrics){0, 0}; // Retorna struct zerada
    }

    IoMetrics metrics = {0, 0};
    char line[256];

    // Lê o arquivo linha por linha
    while (fgets(line, sizeof(line), fp)) {
        
        // Procura por rchar (bytes lidos)
        if (strncmp(line, "rchar:", 6) == 0) {
            // Use %llu para unsigned long long
            sscanf(line, "rchar: %llu", &metrics.rchar);
        }
        // Procura por wchar (bytes escritos)
        else if (strncmp(line, "wchar:", 6) == 0) {
            // Use %llu para unsigned long long
            sscanf(line, "wchar: %llu", &metrics.wchar);
        }
    }

    fclose(fp);
    return metrics;
}