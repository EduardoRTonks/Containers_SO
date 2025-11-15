#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include "monitor.h" 
#include "namespace.h"
#include "cgroup.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <pid> <intervalo_segundos>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    int interval = atoi(argv[2]); 

    if (interval <= 0) {
        fprintf(stderr, "O intervalo deve ser um número positivo.\n");
        return 1;
    }

    long clk_tck = sysconf(_SC_CLK_TCK);
    if (clk_tck <= 0) {
        perror("Erro ao obter sysconf(_SC_CLK_TCK)");
        return 1;
    }

    // --- MUDANÇA 1: Adicionar I/O ao cabeçalho CSV ---
    printf("timestamp_s,pid,cpu_usage_pct,mem_rss_kb,mem_vsz_kb,io_bytes_read,io_bytes_written\n");
    
    long current_time_s = 0; 
    CpuMetrics cpu_before = get_cpu_metrics(pid); 
    // (Não precisamos do I/O "antes" a menos que queiramos taxas,
    // o requisito é só "coletar")

    while (1) {
        sleep(interval); 
        
        CpuMetrics cpu_after = get_cpu_metrics(pid);
        MemoryMetrics mem = get_memory_metrics(pid);
        IoMetrics io = get_io_metrics(pid); // <-- CHAMA A NOVA FUNÇÃO
        current_time_s += interval; 

        unsigned long total_ticks_delta = (cpu_after.utime + cpu_after.stime) - (cpu_before.utime + cpu_before.stime);
        double total_system_ticks_elapsed = (double)interval * clk_tck;
        
        double cpu_percentage = 0.0;
        if (total_system_ticks_elapsed > 0) {
            cpu_percentage = 100.0 * (total_ticks_delta / total_system_ticks_elapsed);
        }

        // --- MUDANÇA 2: Adicionar I/O à linha de dados CSV ---
        // (Use %llu para os valores de I/O)
        printf("%ld,%d,%.2f,%ld,%ld,%llu,%llu\n",
               current_time_s,
               pid,
               cpu_percentage,
               mem.vm_rss_kb,
               mem.vm_size_kb,
               io.rchar,       // <-- Dado novo
               io.wchar);      // <-- Dado novo
        
        cpu_before = cpu_after;
    }

    return 0;
}