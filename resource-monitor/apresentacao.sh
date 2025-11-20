#!/bin/bash

# Cores para o terminal
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function pause(){
    echo -e "${CYAN}>>> Pressione [ENTER] para voltar ao menu...${NC}"
    read
}

while true; do
    clear
    echo -e "${GREEN}=================================================${NC}"
    echo -e "${GREEN}   SISTEMA DE DEFESA RA3 - EDUARDO & RICARDO     ${NC}"
    echo -e "${GREEN}=================================================${NC}"
    echo "1. [Compilação] Limpar e Recompilar (Make)"
    echo "2. [Básico] Monitorar Processo (Temporizado)"
    echo -e "${YELLOW}--- Experimentos Obrigatórios ---${NC}"
    echo "3. [Exp 1] Overhead do Monitor (Monitor vs Sem Monitor)"
    echo "4. [Exp 2] Overhead de Namespaces (Criação de Container)"
    echo "5. [Exp 3] Limite de CPU (Cgroup - 50%)"
    echo "6. [Exp 4] Limite de Memória (Cgroup - 100MB)"
    echo "7. [Exp 5] Limite de I/O (Cgroup - 10MB/s)"
    echo -e "${YELLOW}--- Bônus Visuais ---${NC}"
    echo "8. [Visual] Gráfico em Tempo Real (Estresse CPU)"
    echo "9. [Visual] Gráfico CAÓTICO (CPU + RAM + I/O)"
    echo "0. Sair"
    echo -e "${GREEN}=================================================${NC}"
    read -p "Escolha uma opção: " op

    case $op in
        1)
            echo -e "\n${CYAN}>>> Executando 'make clean && make'...${NC}"
            # Compila silenciando warnings para ficar limpo na apresentação
            make clean > /dev/null 2>&1
            make all CFLAGS="-Wall -Wextra -Wno-format-truncation -g -Iinclude"
            echo "Compilação concluída com sucesso."
            pause
            ;;
        2)
            read -p "Por quantos segundos você quer monitorar? " TEMPO
            echo -e "\n${CYAN}>>> Iniciando processo 'sleep 600' em background...${NC}"
            sleep 600 &
            PID_SLEEP=$!
            echo -e ">>> PID detectado: $PID_SLEEP"
            echo -e ">>> Monitorando por $TEMPO segundos..."
            sleep 1
            timeout $TEMPO ./bin/resource_monitor $PID_SLEEP
            echo -e "\n>>> Tempo esgotado! Encerrando..."
            kill $PID_SLEEP 2>/dev/null
            pause
            ;;
        3)
            echo -e "\n${CYAN}>>> [Exp 1] Medindo Overhead do Monitor...${NC}"
            echo "Este teste compara o tempo de execução de uma tarefa pesada"
            echo "COM e SEM o monitor rodando."
            # Este script não precisa de timeout pois ele tem fim definido
            ./scripts/run_monitor_overhead_test.sh
            pause
            ;;
        4)
            echo -e "\n${CYAN}>>> [Exp 2] Testando Overhead de Namespaces...${NC}"
            sudo ./scripts/run_overhead_test.sh
            pause
            ;;
        5)
            echo -e "\n${CYAN}>>> [Exp 3] Limite de CPU (50%)...${NC}"
            read -p "Duração do teste (segundos): " TEMPO
            echo ">>> A senha de SUDO será solicitada agora."
            sudo timeout -s SIGINT ${TEMPO}s ./scripts/run_cgroup_experiment.sh
            pause
            ;;
        6)
            echo -e "\n${CYAN}>>> [Exp 4] Limite de Memória (100MB)...${NC}"
            read -p "Duração do teste (segundos): " TEMPO
            echo ">>> O processo tentará alocar memória infinitamente."
            sudo timeout -s SIGINT ${TEMPO}s ./scripts/run_memory_limit_test.sh
            pause
            ;;
        7)
            echo -e "\n${CYAN}>>> [Exp 5] Limite de I/O (10MB/s)...${NC}"
            read -p "Duração do teste (segundos): " TEMPO
            echo ">>> Tentando escrever no disco à velocidade máxima..."
            sudo timeout -s SIGINT ${TEMPO}s ./scripts/run_io_limit_test.sh
            pause
            ;;
        8)
            echo -e "\n${CYAN}>>> Iniciando 'test_cpu' (VISUAL)...${NC}"
            ./bin/test_cpu &
            PID_GRAPH=$!
            echo -e ">>> PID detectado: $PID_GRAPH"
            echo -e ">>> Digite sua senha de sudo se solicitado:"
            sudo -v
            sudo python3 scripts/visualize.py $PID_GRAPH
            kill $PID_GRAPH 2>/dev/null
            pause
            ;;
        9)
            echo -e "\n${CYAN}>>> Iniciando 'CARGA TOTAL' (VISUAL CAOS)...${NC}"
            echo ">>> Digite sua senha de sudo AGORA para autorizar o visualizador:"
            sudo -v 
            
            python3 scripts/carga_caos.py &
            PID_CHAOS=$!
            echo -e ">>> PID detectado: $PID_CHAOS"
            echo -e ">>> Abrindo Visualizador..."
            
            sudo python3 scripts/visualize.py $PID_CHAOS
            
            echo -e "\n>>> Matando gerador de carga..."
            kill $PID_CHAOS 2>/dev/null
            pause
            ;;
        0)
            echo "Saindo..."
            pkill -f "test_cpu"
            pkill -f "test_memory"
            pkill -f "test_io"
            pkill -f "sleep 600"
            pkill -f "carga_caos.py"
            exit 0
            ;;
        *)
            echo "Opção inválida."
            sleep 1
            ;;
    esac
done
