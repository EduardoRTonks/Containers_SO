#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <sys/types.h>

/*
 * Lista todos os namespaces (tipo e inode) de um processo específico.
 * [Requisito: Aluno 3]
 */
void list_process_namespaces(pid_t pid);

/*
 * Compara os namespaces entre dois processos.
 * [Requisito: Aluno 3]
 */
void compare_process_namespaces(pid_t pid1, pid_t pid2);

#endif // NAMESPACE_H