/*
 * ==================================================================
 * ARQUIVO DE EXPERIMENTO (Experimento 5: I/O)
 * Aplica um limite de I/O (Leitura/Escrita) a um PID.
 * ==================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cgroup.h" 

// --- Definições do Experimento ---
const char* CGROUP_NAME = "teste-io-10m";
// Limite de 10 Megabytes/s
const long long IO_LIMIT_BYTES_S = 10 * 1024 * 1024; 
// -----------------------------------

int main(int argc, char *argv[]) {
    // Este experimento espera 2 argumentos: <PID> <DeviceMajor:Minor>
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <PID_para_limitar> <DeviceMajor:Minor>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 1234 8:0 (use 'lsblk' para achar seu dispositivo)\n", argv[0]);
        return 1;
    }
    if (getuid() != 0) {
        fprintf(stderr, "ERRO: Este experimento DEVE ser rodado com sudo.\n");
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    const char* device_id = argv[2]; // ex: "8:0"

    printf("Iniciando experimento de Cgroup (I/O) para PID: %d\n", pid);

    if (!cgroup_create(CGROUP_NAME)) {
        fprintf(stderr, "Falha ao criar o cgroup '%s'.\n", CGROUP_NAME);
        return 1;
    }

    if (!cgroup_set_io_limit(CGROUP_NAME, device_id, IO_LIMIT_BYTES_S)) {
        fprintf(stderr, "Falha ao aplicar limite de I/O.\n");
        return 1;
    }

    if (!cgroup_move_process(pid, CGROUP_NAME)) {
        fprintf(stderr, "Falha ao mover PID %d para o cgroup.\n", pid);
        return 1;
    }

    printf("\n======================================================\n");
    printf("Sucesso! O PID %d agora está no cgroup '%s'.\n", pid, CGROUP_NAME);
    printf("Limite de I/O de %lld B/s aplicado.\n", IO_LIMIT_BYTES_S);
    printf("Use o ./bin/resource_monitor %d para validar.\n", pid);
    printf("======================================================\n");

    return 0;
}