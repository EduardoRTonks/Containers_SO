#define _GNU_SOURCE // Necessário para O_DIRECT
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  // Para O_DIRECT
#include <string.h> // Para memset

// O_DIRECT requer alinhamento de memória. Usaremos 4KB.
#define BLOCK_SIZE (4 * 1024)
#define CHUNK_SIZE (10 * 1024 * 1024) // 10 MB
#define FILE_NAME "temp_io_test_direct.dat"

int main() {
    printf("Iniciando stress de I/O (O_DIRECT)... (PID: %d)\n", getpid());

    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_DIRECT, 0644);
    if (fd == -1) {
        perror("Erro ao abrir arquivo com O_DIRECT (verifique permissões)");
        return 1;
    }

    // Aloca um buffer alinhado em 4KB
    void *buffer;
    if (posix_memalign(&buffer, BLOCK_SIZE, CHUNK_SIZE) != 0) {
        perror("posix_memalign falhou");
        close(fd);
        return 1;
    }
    memset(buffer, 0xAA, CHUNK_SIZE);

    printf("Buffer alinhado criado. Iniciando loop de escrita direta...\n");

    while (1) {
        // Escreve 10MB diretamente no disco
        ssize_t bytes_written = write(fd, buffer, CHUNK_SIZE);
        if (bytes_written < 0) {
            perror("write (O_DIRECT) falhou");
        }
        
        // Retorna ao início do arquivo para escrever de novo
        lseek(fd, 0, SEEK_SET);
    }

    // (Nunca chega aqui, mas é boa prática)
    free(buffer);
    close(fd);
    return 0;
}