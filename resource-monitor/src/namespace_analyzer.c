#include "namespace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>   // Para ler diretórios
#include <unistd.h>   // Para readlink

/*
 * Função auxiliar interna para ler um link de namespace específico (ex: "uts", "net")
 * e imprimir seu destino (ex: "uts:[4026531838]")
 */
static void read_ns_link(pid_t pid, const char *ns_name, char *out_link, int out_len) {
    char path[512];
    ssize_t len;

    // Monta o caminho: /proc/123/ns/uts
    snprintf(path, sizeof(path), "/proc/%d/ns/%s", pid, ns_name);
    
    // readlink lê o destino do link simbólico
    len = readlink(path, out_link, out_len - 1);
    
    if (len == -1) {
        snprintf(out_link, out_len, "[acesso negado ou não suportado]");
    } else {
        out_link[len] = '\0'; // Adiciona terminador nulo
    }
}

/*
 * Implementação de list_process_namespaces
 */
void list_process_namespaces(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/ns", pid);

    DIR *d = opendir(path);
    if (d == NULL) {
        perror("Erro ao abrir /proc/[pid]/ns (PID existe?)");
        return;
    }
    
    printf("--- Namespaces para PID: %d ---\n", pid);
    
    struct dirent *dir;
    char link_target[256];
    
    while ((dir = readdir(d)) != NULL) {
        // Ignora os diretórios "." e ".."
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            read_ns_link(pid, dir->d_name, link_target, sizeof(link_target));
            printf("  %-10s: %s\n", dir->d_name, link_target);
        }
    }
    
    closedir(d);
    printf("------------------------------\n");
}

/*
 * Implementação de compare_process_namespaces
 */
void compare_process_namespaces(pid_t pid1, pid_t pid2) {
    char path1[256], path2[256];
    snprintf(path1, sizeof(path1), "/proc/%d/ns", pid1);
    snprintf(path2, sizeof(path2), "/proc/%d/ns", pid2);

    DIR *d = opendir(path1);
    if (d == NULL) {
        // Agora o erro mostrará o PID correto
        char error_msg[100];
        snprintf(error_msg, 100, "Erro ao abrir namespaces do PID %d", pid1);
        perror(error_msg);
        return;
    }

    printf("\n--- Comparando Namespaces [PID: %d] vs [PID: %d] ---\n", pid1, pid2);
    
    struct dirent *dir;
    char link1[256], link2[256];
    
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            read_ns_link(pid1, dir->d_name, link1, sizeof(link1));
            read_ns_link(pid2, dir->d_name, link2, sizeof(link2));
            
            // Compara as strings de destino (ex: "net:[123]" == "net:[123]")
            int sao_iguais = (strcmp(link1, link2) == 0);
            
            printf("  %-10s: %s (%s)\n", dir->d_name, 
                   sao_iguais ? "COMPARTILHADO" : "SEPARADO", 
                   link1);
        }
    }
    
    closedir(d);
    printf("--------------------------------------------------\n");
}