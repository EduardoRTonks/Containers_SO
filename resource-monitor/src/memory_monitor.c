#include "monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MemoryMetrics get_memory_metrics(pid_t pid) {
    char path[256];
    // Cria o caminho para o arquivo /proc/[pid]/status
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        // Trata o erro (processo inexistente ou sem permissão) 
        perror("Erro ao abrir /proc/[pid]/status");
        return (MemoryMetrics){0, 0}; // Retorna struct zerada
    }

    MemoryMetrics metrics = {0, 0};
    char line[256];

    // Lê o arquivo linha por linha
    while (fgets(line, sizeof(line), fp)) {
        
        // Procura por VmSize (Memória Virtual)
        if (strncmp(line, "VmSize:", 7) == 0) {
            sscanf(line, "VmSize: %ld kB", &metrics.vm_size_kb);
        }
        // Procura por VmRSS (Memória Física)
        else if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "VmRSS: %ld kB", &metrics.vm_rss_kb);
        }
    }

    fclose(fp);
    return metrics;
}