# Compilador
CC = gcc

# Flags de Compilação - Obrigatório compilar sem warnings!
# CFLAGS inclui -I para achar os headers em 'include/'
CFLAGS = -Wall -Wextra -g -Iinclude

# Nome do executável final
TARGET = resource_monitor

# Lista de arquivos-fonte (.c)
# Encontra automaticamente todos os .c dentro de 'src/'
SRCS = $(wildcard src/*.c)

# Lista de arquivos-objeto (.o)
# Substitui 'src/%.c' por 'build/%.o'
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

# --- Regras ---

# Regra principal: compilar o programa
all: $(TARGET)

# Regra para linkar o executável final
$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$(TARGET) $(OBJS)
	@echo "Executável [bin/$(TARGET)] criado com sucesso!"

# Regra para compilar arquivos .c em .o
# Cria o diretório 'build' se não existir
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos compilados
clean:
	rm -rf build bin $(TARGET)
	@echo "Arquivos compilados removidos."

# Regra para forçar a recompilação
re: clean all

.PHONY: all clean re