/*
 * ==================================================================
 * ARQUIVO DE EXPERIMENTO (Experimento 4: Memória)
 * Aplica um limite de Memória a um PID.
 * ==================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cgroup.h" 

// --- Definições do Experimento ---
const char* CGROUP_NAME = "teste-mem-100m";
// Limite de 100 Megabytes
const long long MEMORY_LIMIT_BYTES = 100 * 1024 * 1024; 
// -----------------------------------

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <PID_para_limitar>\n", argv[0]);
        return 1;
    }
    if (getuid() != 0) {
        fprintf(stderr, "ERRO: Este experimento DEVE ser rodado com sudo.\n");
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    printf("Iniciando experimento de Cgroup (Memória) para PID: %d\n", pid);

    // 1. Criar o cgroup
    if (!cgroup_create(CGROUP_NAME)) {
        fprintf(stderr, "Falha ao criar o cgroup '%s'.\n", CGROUP_NAME);
        return 1;
    }

    // 2. Aplicar o limite de 100MB
    if (!cgroup_set_memory_limit(CGROUP_NAME, MEMORY_LIMIT_BYTES)) {
        fprintf(stderr, "Falha ao aplicar limite de memória.\n");
        return 1;
    }

    // 3. Mover o PID de teste para dentro do cgroup
    if (!cgroup_move_process(pid, CGROUP_NAME)) {
        // --- ESTA É A LINHA CORRIGIDA ---
        fprintf(stderr, "Falha ao mover PID %d para o cgroup.\n", pid);
        return 1;
    }

    printf("\n======================================================\n");
    printf("Sucesso! O PID %d agora está no cgroup '%s'.\n", pid, CGROUP_NAME);
    printf("Limite de memória de %lld bytes aplicado.\n", MEMORY_LIMIT_BYTES);
    printf("Use o ./bin/resource_monitor %d para validar.\n", pid);
    printf("======================================================\n");

    return 0;
}