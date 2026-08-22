# Simulador RISC 16-bits com Processamento Batch
[![Linguagem](https://img.shields.io/badge/Linguagem-C%2B%2B-blue)]()

**NOTA (Júlia): cada equipe faz seu componente e modificações, na própria branch, quem modificará a main (unificar as branch's) será a INTEGRAÇÃO.
**NOTA (Júlia): caso precise deixar alguma observação para outra equipe, favor deixar aqui NOTA(seu nome) e ou pedir para seu representante enviar no grupo dos capitães.

Este projeto implementa um simulador computacional desenvolvido para a disciplina de Sistemas Operacionais. O objetivo é construir a base de um ecossistema computacional completo, operando inicialmente sob o paradigma de **processamento em lote (batch)**, similar aos primeiros sistemas computacionais da história.

A máquina simulada possui uma arquitetura **RISC (Reduced Instruction Set Computer)** com palavras de **16 bits** e segue o paradigma **Load/Store**, onde operações lógicas e aritméticas ocorrem exclusivamente entre registradores, separando o acesso à memória do processamento dos dados.



## Estrutura de Arquivos (Favor segui-la)

A separação modular do código foi realizada para atender aos requisitos de encapsulamento do projeto:
text

```
.
├── main.cpp                # Ponto de entrada (equipe Integração)
├──── memory/...            # Pasta componente MEMÓRIA
├──── cpu/...               # Pasta componente CPU 
└──── loader/...            # Pasta componente CLOCK-LOADER 

```

# DESCONSIDERAR RESTANTE DO README E SCRIPTS EXISTENTES NA BRANCH MAIN

## Arquitetura do Sistema

O simulador é composto por 4 módulos principais, que se comunicam através de interfaces bem definidas, respeitando o isolamento dos barramentos físicos.

| Módulo | Arquivo(s) | Responsabilidade |
| :--- | :--- | :--- |
| **Definições** | `defines.hpp` | Constantes globais, `Opcodes` e tamanhos da memória. |
| **Memória (RAM/Disco)** | `memory.hpp` / `memory.cpp` | Interface mediadora para leitura/escrita na RAM (com trava de segurança) e simulação de I/O (Disco). |
| **CPU** | `cpu.hpp` / `cpu.cpp` | Contém a ULA, os Registradores, o Program Counter (PC) e a Unidade de Controle (ciclo Fetch-Decode-Execute). |
| **Sistema Operacional** | `so.hpp` / `so.cpp` | Gerenciador de Lote (*Loader*), responsável por carregar os Jobs na RAM, resetar o contexto e executar a fila de tarefas. |

- **Execução Bare-Metal**: A CPU executa instruções diretamente sobre a RAM, sem a intervenção de sistemas operacionais complexos.
- **Trava de Segurança (Kernel Protection)**: Os primeiros 512 endereços da memória são reservados para o "Sistema Operacional" (SO), gerando uma exceção (`SEGFAULT`) caso um job de usuário tente acessá-los.
- **Simulação de I/O**: Acesso ao disco possui penalidade de 50 ciclos de clock por setor percorrido, simulando a latência de dispositivos mecânicos.
- **Modularidade**: O código foi estritamente encapsulado em módulos (CPU, Memória, SO) para facilitar expansões futuras, como a implementação de *Cache* e *Pipeline*.

### O Paradigma Load/Store
Em arquiteturas RISC, as instruções matemáticas (`ADD`, `SUB`, `AND`, `OR`) nunca buscam dados diretamente na memória. Elas operam **apenas sobre registradores**. Portanto, para calcular `C = A + B`, o fluxo é:

1. **LOAD** R0, A  *(Traz A da RAM para o registrador R0)*
2. **LOAD** R1, B  *(Traz B da RAM para o registrador R1)*
3. **ADD**  R2, R0, R1 *(Soma R0 e R1, salva em R2 - operação na ULA)*
4. **STORE** R2, C *(Salva o resultado de R2 de volta na RAM)*
5. **HALT** *(Finaliza)*



## Conjunto de Instruções (ISA)

A arquitetura suporta um conjunto mínimo de instruções de 16 bits. A decodificação é feita por mascaramento de bits:

- **Bits 15–12**: Opcode (4 bits)
- **Bits 11–8**: Registrador de Destino (ou fonte 1)
- **Bits 7–4**: Registrador Fonte 2 (ou parte do endereço)
- **Bits 3–0**: Registrador Fonte 3 (ou complemento do endereço)

| Mnemônico | Opcode (Hex) | Categoria | Descrição |
| :--- | :--- | :--- | :--- |
| **LOAD** | `0x1` | Transferência | `R[Dest] = RAM[Endereço]` |
| **STORE**| `0x3` | Transferência | `RAM[Endereço] = R[Dest]` |
| **ADD**  | `0x2` | ULA (Matemática) | `R[Dest] = R[F1] + R[F2]` |
| **SUB**  | `0x4` | ULA (Matemática) | `R[Dest] = R[F1] - R[F2]` |
| **AND**  | `0x5` | ULA (Lógica) | `R[Dest] = R[F1] & R[F2]` |
| **OR**   | `0x6` | ULA (Lógica) | `R[Dest] = R[F1] \| R[F2]` |
| **BEQ**  | `0x7` | Controle de Fluxo | Se `R[Dest] == R[F1]`, salta para `Endereço` |
| **JUMP** | `0x8` | Controle de Fluxo | Salto incondicional para `Endereço` |
| **HALT** | `0xF` | Sistema | Finaliza a execução do Job atual |



## Hierarquia e Custos (Clock)

O simulador conta com um relógio global que mensura o tempo de execução de cada Job, aplicando penalidades realistas conforme o componente acessado:

- **Acesso à RAM (Ler/EscreverMemoria)**: Custa **1 ciclo de clock**.
- **Acesso ao Disco (LerDisco)**: Custa **50 ciclos de clock por setor** percorrido durante a busca sequencial.
- **Operações na ULA (ADD, SUB, etc.)**: Não custam ciclos de clock adicionais, pois ocorrem internamente na CPU no mesmo ciclo da instrução (modelo didático simplificado).


## Segurança e Trava do Kernel

Para simular a proteção de memória, a constante `OS_RESERVED_MEM = 512` define a área reservada para o Sistema Operacional (endereços **0 a 511**).

As funções `LerMemoria` e `EscreverMemoria` possuem uma verificação rígida:
```cpp
if (endereco < OS_RESERVED_MEM) {
    throw runtime_error("SEGFAULT: Tentativa de acesso em area restrita do SO!");
}
```

Qualquer tentativa de um Job de usuário ler ou escrever nessas posições resulta em uma interrupção crítica e a execução do Job é abortada.

## Como Compilar e Executar
Pré-requisitos

    Compilador C++ com suporte a C++11 ou superior (g++, clang++, etc.).

### Compilação

Navegue até o diretório do projeto e compile todos os arquivos .cpp simultaneamente:
bash

```
g++ -std=c++11 -Wall main.cpp cpu.cpp memory.cpp so.cpp -o simulador
```

### Execução

Após compilar, execute o binário gerado:
bash

```
./simulador
```
 
### Exemplo de Saída Esperada

Ao executar, o terminal exibirá o carregamento do Job 1 (soma), o estado final da CPU, as estatísticas de desempenho e, em seguida, o teste da trava de segurança (Job 2), que gerará uma exceção.
text

```
===================================================
[SIMULADOR RISC 16-BITS] - EXECUCAO EM LOTE
===================================================
Carregando Job 1: soma_simples.txt...
Execucao finalizada (HALT encontrado).
--- ESTADO DA CPU ---
PC: 0x0205   IR: 0xF000
Registradores Gerais:
R0: 0x0005  R1: 0x0007  R2: 0x000C  R3: 0x0000
...
--- ESTATISTICAS DO JOB ---
Instrucoes Executadas: 5
Acessos a Memoria (RAM): 10
Acessos ao Disco (I/O): 0
CLOCK TOTAL DESTE JOB: 10 ciclos
===================================================

Iniciando Job 2 (Simulando ataque ao Kernel)...
[INTERRUPCAO GERADA PELO HARDWARE]: SEGFAULT: Tentativa de leitura em area restrita do SO!
```
