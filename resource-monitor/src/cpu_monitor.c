#include "monitor.h"
#include <stdio.h>
#include <stdlib.h>

CpuMetrics get_cpu_metrics(pid_t pid) {
    char path[256];
    // Cria o caminho para o arquivo /proc/[pid]/stat
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        // Trata o erro (processo inexistente ou sem permissão) 
        perror("Erro ao abrir /proc/[pid]/stat");
        return (CpuMetrics){0, 0}; // Retorna struct zerada
    }

    CpuMetrics metrics = {0, 0};
    
    // Lê e descarta os 13 primeiros campos
    // Captura o 14º (utime) e 15º (stime)
    int result = fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", 
           &metrics.utime, 
           &metrics.stime);

    if (result != 2) {
        // Trata erro de leitura (arquivo mal formatado ou problema ao ler)
        fprintf(stderr, "Erro ao ler campos de /proc/%d/stat\n", pid);
    }

    fclose(fp);
    return metrics;
}