#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> // Para strcmp()
#include "monitor.h"
#include "namespace.h" 
#include "cgroup.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <PID> [--csv]\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    
    // Verifica se o último argumento é "--csv"
    int csv_mode = 0;
    if (argc > 2 && strcmp(argv[argc - 1], "--csv") == 0) {
        csv_mode = 1;
    }

    long HERTZ = sysconf(_SC_CLK_TCK);
    if (HERTZ <= 0) HERTZ = 100;

    if (!csv_mode) {
        printf("Monitorando PID: %d (System HERTZ: %ld)\n\n", pid, HERTZ);
    }

    CpuMetrics cpu_t1 = get_cpu_metrics(pid);
    IoMetrics io_t1 = get_io_metrics(pid);
    CgroupMetrics cgroup_t1 = get_cgroup_metrics(pid);
    NetworkMetrics net_t1 = get_network_metrics(pid);

    if (cpu_t1.utime == 0 && io_t1.rchar == 0) {
        if (!csv_mode)
            fprintf(stderr, "Erro ao ler métricas do PID %d. O processo existe?\n", pid);
        return 1;
    }

    // Envia o cabeçalho do CSV se estiver no modo csv
    if (csv_mode) {
        printf("CPU_PCT,MEM_RSS_KB,IO_R_MBs,IO_W_MBs,CG_CPU_PCT,CG_MEM_MB,CG_IO_R_MBs,CG_IO_W_MBs\n");
        fflush(stdout); // Garante que o Python receba o cabeçalho
    }

    while (1) {
        double INTERVALO_SEGUNDOS = 1.0;
        sleep(INTERVALO_SEGUNDOS);

        // Coleta T2
        CgroupMetrics cgroup_t2 = get_cgroup_metrics(pid);
        CpuMetrics cpu_t2 = get_cpu_metrics(pid);
        IoMetrics io_t2 = get_io_metrics(pid);
        MemoryMetrics mem_t2 = get_memory_metrics(pid);
        NetworkMetrics net_t2 = get_network_metrics(pid);

        // Deltas
        unsigned long long delta_cgroup_ns = cgroup_t2.cpu_usage_ns - cgroup_t1.cpu_usage_ns;
        unsigned long delta_jiffies = (cpu_t2.utime + cpu_t2.stime) - (cpu_t1.utime + cpu_t1.stime);
        unsigned long long delta_read_bytes = io_t2.rchar - io_t1.rchar;
        unsigned long long delta_write_bytes = io_t2.wchar - io_t1.wchar;
        unsigned long long delta_rx_bytes = net_t2.rx_bytes - net_t1.rx_bytes;
        unsigned long long delta_tx_bytes = net_t2.tx_bytes - net_t1.tx_bytes;
        unsigned long long delta_cgroup_io_r = cgroup_t2.blkio_read_bytes - cgroup_t1.blkio_read_bytes;
        unsigned long long delta_cgroup_io_w = cgroup_t2.blkio_write_bytes - cgroup_t1.blkio_write_bytes;

        // Cálculos
        double cgroup_cpu_percent = ((double)delta_cgroup_ns / 1e9 / INTERVALO_SEGUNDOS) * 100.0;
        double cpu_percent = ((double)delta_jiffies / HERTZ / INTERVALO_SEGUNDOS) * 100.0;
        double read_MBs = (double)delta_read_bytes / (1024.0 * 1024.0) / INTERVALO_SEGUNDOS;
        double write_MBs = (double)delta_write_bytes / (1024.0 * 1024.0) / INTERVALO_SEGUNDOS;
        double read_MBs_net = (double)delta_rx_bytes / (1024.0 * 1024.0) / INTERVALO_SEGUNDOS;
        double write_MBs_net = (double)delta_tx_bytes / (1024.0 * 1024.0) / INTERVALO_SEGUNDOS;
        double cgroup_read_MBs = (double)delta_cgroup_io_r / (1024.0 * 1024.0) / INTERVALO_SEGUNDOS;
        double cgroup_write_MBs = (double)delta_cgroup_io_w / (1024.0 * 1024.0) / INTERVALO_SEGUNDOS;
        double cgroup_mem_mb = cgroup_t2.memory_usage_bytes / (1024.0 * 1024.0);

        if (csv_mode) {
            // Se modo CSV, imprime apenas a linha de dados
            printf("%.2f,%ld,%.2f,%.2f,%.2f,%.1f,%.2f,%.2f\n",
                   cpu_percent, mem_t2.vm_rss_kb, read_MBs, write_MBs,
                   cgroup_cpu_percent, cgroup_mem_mb, cgroup_read_MBs, cgroup_write_MBs);
                   
            // --- ESTA É A CORREÇÃO CRÍTICA ---
            // Força o C a enviar a linha de dados para o Python AGORA
            fflush(stdout); 
            // ---------------------------------
            
        } else {
            // Modo normal (visualizador de terminal)
            printf("================================\n");
            printf("PID: %d\n", pid);
            printf("CPU: %.2f %%\n", cpu_percent);
            printf("MEM (RSS): %ld KB (%.1f MB)\n", mem_t2.vm_rss_kb, mem_t2.vm_rss_kb / 1024.0);
            printf("MEM (Virt): %ld KB\n", mem_t2.vm_size_kb);
            printf("I/O Leitura: %.2f MB/s\n", read_MBs);
            printf("I/O Escrita: %.2f MB/s\n", write_MBs);
            printf("--- Cgroup Metrics (PID: %d) ---\n", pid);
            printf("Cgroup CPU: %.2f %% (Total: %.2f s)\n", cgroup_cpu_percent, cgroup_t2.cpu_usage_ns / 1e9);
            printf("Cgroup Mem: %.1f MB\n", cgroup_mem_mb);
            printf("Cgroup PIDs: %ld\n", cgroup_t2.pids_current);
            printf("Cgroup I/O R: %.2f MB/s\n", cgroup_read_MBs);
            printf("Cgroup I/O W: %.2f MB/s\n", cgroup_write_MBs);
            printf("Rede Recebido (Rx): %.2f MB/s\n", read_MBs_net);
            printf("Rede Enviado (Tx): %.2f MB/s\n", write_MBs_net);
            printf("\n--- Namespaces (PID: %d) ---\n", pid);
            list_process_namespaces(pid);
            printf("================================\n");
        }

        // Atualizar T1
        cpu_t1 = cpu_t2;
        io_t1 = io_t2;
        cgroup_t1 = cgroup_t2;
        net_t1 = net_t2;
    }
    return 0;
}