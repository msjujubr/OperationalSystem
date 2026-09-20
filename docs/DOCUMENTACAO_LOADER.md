# Documentação Técnica — Loader em Lote e Motor de Clock

> Documentação do subsistema **Clock & Loader** do Simulador RISC 16-bits, detalhando o fluxo de carregamento batch, a contagem de ciclos de relógio e a geração do relatório `output.dat`.

---

## 👥 Equipe Responsável (Subgrupo Clock & Loader)

| Integrante | Função / Responsabilidade |
| :--- | :--- |
| **João Vitor** | Líder do Subgrupo & Interfaces Gerais |
| **Ahmed** | Motor do Clock (`clock_core`) |
| **Hugo** | Telemetria & Métricas (`metrics`) |
| **Thallys** | Relatório & Persistência (`reporter` e `output.dat`) |
| **Lucas Roseno** | Parser de Arquivos & Acesso a Disco (`parser`) |
| **Luiz Fernando** | Carga e Mapeamento na RAM (`ram_loader`) |
| **João Pedro** | Orquestrador e Gerenciador de Lote (`batch_manager`) |
| **Bernardo** | Cargas de Teste & Documentação Técnica (`tests/` e `docs/`) |

---

## 1. Arquitetura do Módulo Clock & Loader

O módulo é composto por 7 componentes internos que se comunicam de forma hierárquica:

```
main.cpp
  └─ BatchManager::executarLote()
       ├─ JobParser::carregarArquivo()    → Lê o .txt e extrai instruções/dados
       ├─ ClockCore::reset()              → Zera o relógio do job
       ├─ MetricsTracker::iniciarJob()    → Abre a sessão de telemetria
       ├─ RAMLoader::carregarNaRAM()      → Copia instruções e dados para a RAM física
       ├─ CPU::step() [em laço]           → Execução instrução a instrução
       ├─ MetricsTracker::obterMetricas() → Consolida estatísticas
       └─ Reporter::exibirTerminal()      → Imprime no terminal
           Reporter::salvarOutputDat()    → Grava no output.dat
```

### 1.1 Arquivos do Módulo

| Arquivo              | Responsabilidade                                          |
| :------------------- | :-------------------------------------------------------- |
| `clock_types.hpp`    | Estruturas `JobData` e `JobMetrics` (tipos de dados)      |
| `clock_core.hpp/.cpp`| Motor do relógio global: penalidades de RAM e Disco       |
| `metrics.hpp/.cpp`   | Telemetria: coleta instruções, acessos RAM/Disco por Job  |
| `parser.hpp/.cpp`    | Leitura de arquivos `.txt`, extração de instruções/dados  |
| `ram_loader.hpp/.cpp`| Mapeamento e cópia de instruções/dados na RAM física      |
| `reporter.hpp/.cpp`  | Formatação de saída (terminal + `output.dat`)             |
| `batch_manager.hpp/.cpp` | Orquestrador do ciclo de vida em lote               |

---

## 2. Fluxo Completo de Execução em Lote

### 2.1 Diagrama de Sequência

```
┌──────────┐  ┌───────────┐  ┌──────────┐  ┌─────────┐  ┌─────┐  ┌──────────┐
│  main()  │  │BatchManager│  │ JobParser │  │RAMLoader│  │ CPU │  │ Reporter │
└────┬─────┘  └─────┬──────┘  └────┬──────┘  └────┬────┘  └──┬──┘  └────┬─────┘
     │              │              │              │          │          │
     │─executarLote─▶              │              │          │          │
     │              │              │              │          │          │
     │              │ Para cada Job na fila:      │          │          │
     │              │──carregarArquivo──▶          │          │          │
     │              │◀──JobData────────│          │          │          │
     │              │                             │          │          │
     │              │  ClockCore::reset()         │          │          │
     │              │  MetricsTracker::iniciarJob()│         │          │
     │              │  CPU::reset()               │          │          │
     │              │                             │          │          │
     │              │  tickDisco(1) [carga inicial]│         │          │
     │              │                             │          │          │
     │              │──carregarNaRAM──────────────▶          │          │
     │              │◀──true/false────────────────│          │          │
     │              │                                        │          │
     │              │  while (!cpu.isHalted()):              │          │
     │              │────────────────────────────cpu.step()──▶          │
     │              │◀──────────────────────────────────────│          │
     │              │  MetricsTracker::registrarInstrucao()  │          │
     │              │                                        │          │
     │              │  MetricsTracker::capturarEstadoCPU()   │          │
     │              │  MetricsTracker::obterMetricas()       │          │
     │              │                                                  │
     │              │──exibirTerminal──────────────────────────────────▶
     │              │──salvarOutputDat─────────────────────────────────▶
     │              │                                                  │
```

### 2.2 Passo a Passo Detalhado

#### Fase 1 — Leitura do Job (Disco Virtual → Parser)

1. O `BatchManager` recebe a lista de caminhos dos arquivos `.txt`
2. Para cada arquivo, o `JobParser::carregarArquivo()`:
   - Abre o arquivo de texto
   - Ignora comentários (`//`) e linhas vazias
   - Separa seções `.text` (instruções) e `.data` (variáveis)
   - Converte valores hexadecimais para `uint16_t`
   - Retorna a estrutura `JobData` preenchida
3. Penalidade: **1 acesso ao disco**, custando **50 ciclos** de clock (simulação da latência de I/O)

#### Fase 2 — Reset de Contexto

Antes de cada job, o sistema reinicia completamente:

| Componente       | Ação                                 |
| :--------------- | :----------------------------------- |
| `ClockCore`      | `ciclosJob = 0`                      |
| `MetricsTracker` | Nova sessão (`JobMetrics` zerado)    |
| `clockGlobal`    | `= 0` (variável global de memory.hpp)|
| `acessosMemoria` | `= 0`                               |
| `acessosDisco`   | `= 0`                               |
| `instExecutadas` | `= 0`                               |
| `CPU`            | PC = 512, IR = 0, R0–R7 = 0, halt = false |

> **Importante:** O reset garante que nenhum resíduo de um job anterior influencie o próximo. Isso é o princípio fundamental do processamento em lote.

#### Fase 3 — Carga na RAM (RAMLoader)

O `RAMLoader::carregarNaRAM()` realiza:

1. **Instruções:** Copia sequencialmente a partir do endereço base (512)
   ```
   RAM[512 + 0] = instrução 0
   RAM[512 + 1] = instrução 1
   ...
   RAM[512 + N-1] = instrução N-1
   ```

2. **Variáveis:** Posiciona nos endereços relativos declarados na seção `.data`
   ```
   RAM[512 + endereço_relativo] = valor
   ```

3. **Validação:** Retorna `false` se algum endereço exceder o limite da RAM (65.536)

#### Fase 4 — Execução (CPU em laço)

```cpp
while (!cpu.isHalted()) {
    if (instrucoesDoJob >= 1.000.000) {
        // Proteção contra loop infinito → aborta o job
        break;
    }
    cpu.step();          // Busca → Decodificação → Execução
    MetricsTracker::registrarInstrucao();
}
```

- Cada `cpu.step()` executa **um ciclo completo** de Fetch-Decode-Execute
- A instrução `HALT` (opcode 0xF) define `haltStatus = true` e encerra o laço
- O limite de 1 milhão de instruções protege o lote contra jobs mal formados

#### Fase 5 — Relatório e Persistência

1. `MetricsTracker::capturarEstadoCPU()` salva o snapshot final da CPU (PC, IR, R0–R7)
2. `Reporter::exibirTerminal()` imprime no `stdout` formatado
3. `Reporter::salvarOutputDat()` grava em `output.dat` (modo append)

---

## 3. Motor de Relógio (ClockCore)

O `ClockCore` centraliza toda a contabilidade temporal do simulador.

### 3.1 Custos de Clock

| Operação                          | Custo               | Método                    |
| :-------------------------------- | :------------------- | :------------------------ |
| Acesso à RAM (leitura/escrita)    | **+1 ciclo**         | `ClockCore::tickRAM()`    |
| Busca de instrução (fetch)        | **+1 ciclo**         | `ClockCore::tickBusca()`  |
| Acesso ao Disco (por setor)       | **+50 ciclos/setor** | `ClockCore::tickDisco(n)` |
| Operações na ULA (ADD, SUB, etc.) | **0 ciclos**         | *(ocorrem no mesmo ciclo)* |

### 3.2 Contadores

| Contador            | Escopo        | Descrição                                    |
| :------------------ | :------------ | :------------------------------------------- |
| `ciclosJob`         | Por Job       | Zerado a cada novo job                       |
| `ciclosTotalGlobal` | Acumulativo   | Total de ciclos de todos os jobs da sessão   |

### 3.3 Cálculo do Clock Total de um Job

O clock total reportado é a soma de:

```
Clock Total = (Acessos à RAM × 1) + (Setores de Disco × 50) + Ciclos extras
```

**Exemplo (Job 1 — Soma):**
- 1 acesso ao disco na carga inicial: 50 ciclos
- 8 acessos à RAM durante execução: 8 ciclos
- Total: **58 ciclos**

---

## 4. Área Reservada do SO (Proteção de Memória)

```
Endereços 0x0000 a 0x01FF (0 a 511) → Reservados ao Sistema Operacional
```

### 4.1 Mecanismo de Proteção

As funções `LerMemoria()` e `EscreverMemoria()` verificam rigidamente:

```cpp
if (endereco < OS_RESERVED_MEM) {   // OS_RESERVED_MEM = 512
    throw runtime_error("SEGFAULT: Tentativa de acesso em area restrita do SO!");
}
```

Se um job tentar acessar endereços na faixa reservada:
1. Uma exceção `std::runtime_error` é lançada
2. O `BatchManager` captura a exceção
3. O job é marcado com `erroKernel = true`
4. A execução do **job corrente** é abortada
5. O lote **continua** para o próximo job

> **Princípio do Lote:** A falha de um job não derruba o sistema. O processamento em lote é tolerante a falhas individuais.

---

## 5. Formato do output.dat

O arquivo `output.dat` é gerado automaticamente pelo `Reporter` e contém o relatório persistente de todos os jobs executados. O arquivo é aberto em modo **append** (`std::ios::app`), acumulando resultados.

### 5.1 Exemplo de Saída

```
--- ESTADO DA CPU ---
PC: 0x0205 IR: 0xF000
Registradores Gerais:
R0: 0x0005   R1: 0x0007   R2: 0x000C   R3: 0x0000
R4: 0x0000   R5: 0x0000   R6: 0x0000   R7: 0x0000
--- ESTATISTICAS DO JOB ---
Instrucoes Executadas: 5
Acessos a Memoria (RAM): 8
Acessos ao Disco (I/O): 1
CLOCK TOTAL DESTE JOB: 58 ciclos
===================================================
```

### 5.2 Campos do Relatório

| Campo                    | Descrição                                       |
| :----------------------- | :---------------------------------------------- |
| `PC`                     | Valor final do Program Counter (hex, 16 bits)   |
| `IR`                     | Última instrução decodificada (hex, 16 bits)    |
| `R0–R7`                 | Estado final dos 8 registradores (hex, 16 bits) |
| `Instrucoes Executadas`  | Total de instruções processadas pelo step()     |
| `Acessos a Memoria (RAM)`| Quantidade de leituras/escritas na RAM          |
| `Acessos ao Disco (I/O)` | Quantidade de acessos ao disco simulado         |
| `CLOCK TOTAL DESTE JOB`  | Soma total de ciclos de relógio do job          |

---

## 6. Estruturas de Dados Internas

### 6.1 JobData (Entrada do Parser)

```cpp
struct JobData {
    int id;                                          // ID sequencial do Job
    std::string nomeArquivo;                         // Caminho do arquivo .txt
    std::vector<uint16_t> instrucoes;                // Código do programa
    std::vector<std::pair<uint16_t, uint16_t>> variaveis; // {endereço_relativo, valor}
};
```

### 6.2 JobMetrics (Saída da Telemetria)

```cpp
struct JobMetrics {
    int idJob;                    // ID do Job
    std::string nomeJob;          // Nome do arquivo
    uint32_t instrucoesExecutadas;// Total de instruções
    uint32_t acessosRAM;          // Acessos à memória
    uint32_t acessosDisco;        // Acessos ao disco
    uint32_t clockTotal;          // Ciclos totais do job
    uint16_t pcFinal;             // PC final
    uint16_t irFinal;             // IR final
    uint16_t registradores[8];    // Estado de R0–R7
    bool erroKernel;              // Flag de erro (SEGFAULT / abort)
};
```

---

## 7. Como Compilar e Executar

### 7.1 Compilação

```bash
g++ -Wall -Wextra -std=c++17 -I. -Isrc -Iloader \
    src/memory.cpp src/cpu.cpp src/so.cpp \
    loader/*.cpp main.cpp \
    -o simulador
```

### 7.2 Execução (jobs padrão)

```bash
./simulador
```

Executa automaticamente os 3 jobs padrão definidos em `main.cpp`:
1. `tests/job1_soma.txt`
2. `tests/job2_vetor.txt`
3. `tests/job3_loop.txt`

### 7.3 Execução (jobs customizados)

```bash
./simulador tests/job1_soma.txt tests/meu_job.txt
```

Passa arquivos específicos via argumentos de linha de comando.

---

## 8. Resultados dos Testes Validados

### 8.1 Job 1 — Soma Simples

| Métrica               | Valor     |
| :-------------------- | :-------- |
| Instruções Executadas | 5         |
| Acessos à RAM         | 8         |
| Acessos ao Disco      | 1         |
| Clock Total           | 58 ciclos |
| PC Final              | 0x0205    |
| R2 (resultado C=A+B)  | 0x000C (12) |

### 8.2 Job 2 — Operações Lógicas e Aritméticas

| Métrica               | Valor     |
| :-------------------- | :-------- |
| Instruções Executadas | 6         |
| Acessos à RAM         | 9         |
| Acessos ao Disco      | 1         |
| Clock Total           | 59 ciclos |
| PC Final              | 0x0206    |
| R2 (SUB: 20-8)        | 0x000C (12) |
| R3 (AND: 20 & 8)      | 0x0000 (0) |

### 8.3 Job 3 — Laço de Repetição

| Métrica               | Valor     |
| :-------------------- | :-------- |
| Instruções Executadas | 17        |
| Acessos à RAM         | 23        |
| Acessos ao Disco      | 1         |
| Clock Total           | 73 ciclos |
| PC Final              | 0x0209    |
| R3 (Contador final)   | 0x0000 (0) |
| Iterações do laço     | 3         |

---

*Documento gerado para o subgrupo Clock & Loader — Disciplina de Sistemas Operacionais*
