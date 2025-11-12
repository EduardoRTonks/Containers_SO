#ifndef NAMESPACE_H
#define NAMESPACE_H

// Inclua headers necessários (ex: para pid_t)
#include <sys/types.h>

/*
 * Lista os namespaces de um processo específico.
 */
void list_process_namespaces(pid_t pid);

/*
 * Compara os namespaces entre dois processos.
 */
void compare_process_namespaces(pid_t pid1, pid_t pid2);

#endif // NAMESPACE_H