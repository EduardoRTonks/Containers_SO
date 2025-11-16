#!/usr/bin/env python3
"""
Script de Visualização em Tempo Real (Opcional / Pontos Extras)

Este script executa o 'resource_monitor' no modo CSV,
lê seus dados em tempo real e plota em um gráfico.
"""

import sys
import subprocess
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# --- Configuração ---
PID_TO_MONITOR = sys.argv[1] if len(sys.argv) > 1 else None
# Comando para rodar o monitor
MONITOR_COMMAND = ["sudo", "./bin/resource_monitor", PID_TO_MONITOR, "--csv"]
# --------------------

if PID_TO_MONITOR is None:
    print(f"Erro: Você precisa passar o PID.")
    print(f"Uso: python3 {sys.argv[0]} <PID>")
    sys.exit(1)

print(f"Iniciando monitor para PID: {PID_TO_MONITOR}...")
print("Comando:", ' '.join(MONITOR_COMMAND))

try:
    # Inicia o processo 'resource_monitor'
    process = subprocess.Popen(
        MONITOR_COMMAND,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1 # Line-buffered
    )

    # Lê o cabeçalho (primeira linha)
    header_line = process.stdout.readline()
    if not header_line:
        print("Erro: Não recebi nenhum dado do resource_monitor.")
        stderr_output = process.stderr.read()
        print("Saída de Erro do Monitor:")
        print(stderr_output)
        if "sudo" in stderr_output:
            print("\n>> DICA: O script de visualização precisa rodar como root (sudo python3 ...)")
        sys.exit(1)
        
    headers = header_line.strip().split(',')
    print(f"Colunas detectadas: {headers}")

    # Estrutura para guardar os dados
    data = {h: [] for h in headers}
    
    # Configura os gráficos
    # Vamos plotar 4 gráficos
    fig, axes = plt.subplots(4, 1, figsize=(10, 10), sharex=True)
    fig.suptitle(f"Monitoramento em Tempo Real - PID: {PID_TO_MONITOR}")
    
    (ax_cpu, ax_mem, ax_io, ax_cgroup) = axes
    
    ax_cpu.set_ylabel("CPU (%)")
    ax_mem.set_ylabel("Memória RSS (KB)")
    ax_io.set_ylabel("I/O Processo (MB/s)")
    ax_cgroup.set_ylabel("Cgroup I/O (MB/s)")
    ax_cgroup.set_xlabel("Tempo (s)")

    def update(frame):
        try:
            # Lê a próxima linha de dados
            line = process.stdout.readline()
            if not line:
                return
            
            values = line.strip().split(',')
            
            if len(values) != len(headers):
                print(f"Ignorando linha mal formatada: {line.strip()}")
                return

            # Adiciona os novos dados
            for i, h in enumerate(headers):
                data[h].append(float(values[i]))
            
            # Limita os gráficos aos últimos 100 pontos
            for h in headers:
                if len(data[h]) > 100:
                    data[h].pop(0)
            
            # --- Limpa e redesenha os gráficos ---
            ax_cpu.clear()
            ax_cpu.plot(data['CPU_PCT'], label='CPU Processo', color='blue')
            ax_cpu.plot(data['CG_CPU_PCT'], label='CPU Cgroup', color='cyan', linestyle='--')
            ax_cpu.legend(loc='upper left')
            ax_cpu.set_ylim(0, 105)

            ax_mem.clear()
            ax_mem.plot(data['MEM_RSS_KB'], label='Memória RSS', color='green')
            ax_mem.legend(loc='upper left')

            ax_io.clear()
            ax_io.plot(data['IO_R_MBs'], label='I/O Leitura', color='orange')
            ax_io.plot(data['IO_W_MBs'], label='I/O Escrita', color='red')
            ax_io.legend(loc='upper left')

            ax_cgroup.clear()
            ax_cgroup.plot(data['CG_IO_R_MBs'], label='Cgroup I/O R', color='orange', linestyle='--')
            ax_cgroup.plot(data['CG_IO_W_MBs'], label='Cgroup I/O W', color='red', linestyle='--')
            ax_cgroup.legend(loc='upper left')
            
            # Renomeia os eixos (eles são limpos a cada frame)
            ax_cpu.set_ylabel("CPU (%)")
            ax_mem.set_ylabel("Memória RSS (KB)")
            ax_io.set_ylabel("I/O Processo (MB/s)")
            ax_cgroup.set_ylabel("Cgroup I/O (MB/s)")
            ax_cgroup.set_xlabel("Tempo (s)")


        except Exception as e:
            print(f"Erro no update: {e}")

    # Inicia a animação
    # --- ESTA É A LINHA CORRIGIDA ---
    ani = FuncAnimation(fig, update, interval=1000)
    # ---------------------------------
    
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    
    try:
        plt.show() # Mostra a janela do gráfico
    except Exception:
        print("Gráfico fechado.")

finally:
    print("Encerrando monitor...")
    process.terminate() # Termina o processo C
    process.wait()