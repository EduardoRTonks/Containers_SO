#!/bin/bash
# Este script executa o teste de overhead de namespace (Aluno 3)

set -e

# 1. Compilar o teste de overhead (main_overhead_test.c)
# (O binário 'overhead_test' que você enviou já está compilado, 
# mas é uma boa prática compilar de novo caso o .c mude)
echo "--- Compilando o teste de overhead (main_overhead_test.c)... ---"
gcc -o overhead_test tests/main_overhead_test.c -Iinclude -Wall -g

# 2. Mover para a pasta de scripts (se não estiver lá)
mv overhead_test scripts/ 2>/dev/null || true

# 3. Verificar permissões
echo "--- Verificando permissões (sudo é necessário para 'unshare')... ---"
if [ "$EUID" -ne 0 ]; then
  echo "ERRO: Este script DEVE ser rodado com sudo."
  exit 1
fi

# 4. Executar o teste
echo "--- Executando o teste de overhead... ---"
sudo ./scripts/overhead_test