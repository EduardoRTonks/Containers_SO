-----

# 📖 README: Monitor de Recursos Linux

Este projeto é uma ferramenta de linha de comando para monitorar o que um processo (programa) específico está fazendo no seu sistema Linux. Ele exibe em tempo real o uso de **CPU**, **Memória** e **I/O (Leitura/Escrita)**.

-----

## 🚀 Guia Rápido para Teste (para Leigos)

Você vai precisar de **dois terminais** abertos ao mesmo tempo:

  * **Terminal 1:** Será a nossa "cobaia" (um programa para monitorar).
  * **Terminal 2:** Onde vamos rodar nosso monitor.

-----

### 1\. Pré-requisitos (Garantindo as Ferramentas)

Antes de tudo, precisamos garantir que seu sistema tenha as ferramentas de compilação.

1.  Abra seu terminal.
2.  Copie e cole os comandos abaixo, um de cada vez, e pressione Enter.
      * Isso atualiza a lista de programas:
        ```bash
        sudo apt update
        ```
      * Isso instala o compilador C e o `make`:
        ```bash
        sudo apt install build-essential
        ```

### 2\. Compilando o Projeto

Agora que temos as ferramentas, vamos compilar o código.

1.  Navegue até a pasta onde você salvou os arquivos do projeto. (Substitua `nome-da-pasta-do-projeto` pelo nome real).
    ```bash
    cd nome-da-pasta-do-projeto
    ```
2.  Para garantir uma compilação limpa, digite:
    ```bash
    make clean
    ```
3.  Agora, para compilar o programa:
    ```bash
    make
    ```
4.  Se tudo correu bem, você terá um novo arquivo executável na pasta. Vamos assumir que ele se chama `resource_monitor`.

### 3\. Executando o Teste

Siga exatamente estes passos:

#### 🎯 Passo 1: Abrir o Terminal 1 (A "Cobaia")

Abra uma **nova** janela de terminal. Nela, vamos criar um processo "cobaia" que não faz nada além de dormir.

Digite o seguinte comando e pressione Enter:

```bash
sleep 600
```

> **O que aconteceu?** Este terminal agora está "parado" executando o comando `sleep` (dormir) por 10 minutos (600 segundos). Deixe este terminal aberto e não mexa nele.

#### 🎯 Passo 2: Abrir o Terminal 2 (O "Monitor")

Abra uma **segunda** janela de terminal. Nela, vamos fazer todo o resto.

1.  Navegue até a pasta do projeto (o mesmo lugar do Passo 2 de compilação):
    ```bash
    cd nome-da-pasta-do-projeto
    ```

#### 🎯 Passo 3: Descobrir o "RG" da Cobaia (o PID)

No **Terminal 2**, precisamos descobrir o número de identificação (o PID) do nosso comando `sleep` que está rodando no Terminal 1.

Digite este comando:

```bash
pgrep sleep
```

> **O que aconteceu?** O terminal vai lhe responder com um número, por exemplo: `12345`.
>
> Esse número é o **PID** (Process ID) do seu comando `sleep`. Anote ou memorize esse número.

#### 🎯 Passo 4: Rodar o Monitor\!

Finalmente, ainda no **Terminal 2**, vamos rodar nosso monitor e apontá-lo para o PID que encontramos.

**Se o seu PID foi `12345`**, o comando seria:

```bash
./resource_monitor 12345
```

> **Nota:** O `./` no início é importante\! Ele diz ao terminal "execute o programa `resource_monitor` que está *exatamente nesta pasta*".

#### 🎯 Passo 5: Ver os Resultados

O seu **Terminal 2** agora deve começar a imprimir o status do processo "sleep", atualizando a cada segundo:

```
Monitorando PID: 12345 (System HERTZ: 100)

================================
PID: 12345
CPU: 0.00 %
MEM (RSS): 768 KB (0.8 MB)
MEM (Virt): 2632 KB
I/O Leitura: 0.00 MB/s
I/O Escrita: 0.00 MB/s
================================
...
```

(Como o `sleep` não faz nada, é normal que todos os valores sejam 0% ou muito baixos).

**Para parar o monitor**, volte ao **Terminal 2** e pressione as teclas `Ctrl + C` ao mesmo tempo.