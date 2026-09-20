# Documentação Arquitetural e Manual de Referência: Subsistema de CPU

Este documento descreve a arquitetura, o funcionamento interno e a interface de programação da CPU desenvolvida para o simulador da disciplina de Sistemas Operacionais. O objetivo é fornecer a documentação necessária para as equipes de Integração e Sistema Operacional.

---

## 1. Fundamentos da Arquitetura

O módulo da CPU segue a arquitetura **RISC (Reduced Instruction Set Computer)** com palavras de **16 bits**. A base desse modelo é o paradigma **Load/Store**:
* A CPU **não processa dados diretamente na Memória RAM**. 
* As operações matemáticas e lógicas ocorrem apenas entre os registradores. O acesso à memória externa é feito exclusivamente pelas instruções de leitura (*Load*) e escrita (*Store*).

---

## 2. Organização Estrutural (Módulos)

A arquitetura interna da CPU é dividida em três componentes principais, focando no encapsulamento (`src/cpu/`):

### 2.1. Unidade de Controle (UC)
Controla o fluxo de execução da CPU. A UC não faz cálculos matemáticos. Ela busca instruções na Memória, decodifica os *opcodes* e envia comandos para a ULA ou para a Interface de Memória, coordenando o ciclo de instrução.

### 2.2. Unidade Lógica e Aritmética (ULA)
É o componente de cálculo. A ULA executa apenas operações matemáticas (Soma, Subtração) e lógicas (AND, OR). A Unidade de Controle envia os operandos e a operação, e a ULA devolve o resultado, verificando possíveis problemas como *overflow*.

### 2.3. Banco de Registradores
É a memória rápida e interna do processador, fundamental para o fluxo de dados:
- **Registradores de Uso Geral (`R0` a `R7`):** Oito registradores para armazenar variáveis temporárias e operandos. O `R0` pode ser lido e escrito normalmente.
- **PC (Program Counter):** Registrador que guarda o endereço da próxima instrução a ser executada.
- **IR (Instruction Register):** Registrador que guarda a instrução atual sendo decodificada.

---

## 3. O Ciclo de Instrução

O processamento ocorre através de três etapas repetidas pela Unidade de Controle:

1. **Busca (Fetch):** A CPU lê o registrador `PC` para encontrar o endereço atual na RAM. A instrução desse endereço é copiada para o `IR` e o `PC` é incrementado.
2. **Decodificação (Decode):** A instrução de 16 bits no `IR` é dividida (*bit masking*). A Unidade de Controle identifica o comando (*Opcode*) e os operandos (registradores ou endereços imediatos).
3. **Execução (Execute):** A instrução é executada. A UC aciona a ULA para cálculos, usa a RAM para *Load/Store*, ou altera o *Program Counter* para saltos lógicos.

---

## 4. Manual do Assembly (Conjunto de Instruções)

Para escrever programas (*Jobs*) compatíveis com a CPU, as instruções devem ter **16 bits**, seguindo a tabela abaixo. 

### 4.1. Tabela de Opcodes

| Mnemônico | Opcode (Hex) | Categoria | Descrição |
| :--- | :---: | :--- | :--- |
| **LOAD** | `0x1` | Memória | Carrega um valor da RAM para um registrador interno. |
| **ADD** | `0x2` | ULA | Soma dois registradores. |
| **STORE** | `0x3` | Memória | Salva o valor de um registrador na RAM. |
| **SUB** | `0x4` | ULA | Subtrai dois registradores. |
| **AND** | `0x5` | Lógica | Operação bit-a-bit AND. |
| **OR** | `0x6` | Lógica | Operação bit-a-bit OR. |
| **BEQ** | `0x7` | Fluxo | *Branch if Equal*: Desvia a execução se dois registradores forem iguais. |
| **JUMP** | `0x8` | Fluxo | Salto Incondicional: Desvia a execução direto para o endereço informado. |
| **HALT** | `0xF` | Sistema | Finaliza a execução do *Job*. |

### 4.2. Layout de Instruções (16 bits)
A estrutura da palavra de 16 bits muda conforme a categoria da instrução:
- **Aritméticas e Lógicas (ADD, SUB, AND, OR):** `[Opcode: 4 bits] [Destino: 4 bits] [Fonte 1: 4 bits] [Fonte 2: 4 bits]`
- **Transferência e Condicional (LOAD, STORE, BEQ):** `[Opcode: 4 bits] [Destino: 4 bits] [Endereço de Memória: 8 bits]`
- **Salto Incondicional (JUMP):** `[Opcode: 4 bits] [Endereço de Memória: 12 bits]`

---

## 5. Guia de Integração

O código abaixo mostra como instanciar e usar a CPU no arquivo principal (`main.cpp`), guiando as equipes de Integração e de SO.

```cpp
#include "cpu/cpu.hpp"

// 1. Instanciação da máquina virtual
CPU simulador;

// 2. Inicialização do Program Counter apontando para o início do programa (ex: endereço 512)
simulador.setPC(512);

// 3. Loop principal (Execução em Batch)
while (!simulador.isHalted()) {
    // Executa um ciclo completo de Busca, Decodificação e Execução
    simulador.step(); 
}

// 4. Imprime o estado final dos registradores e métricas
simulador.imprimirEstado();
```


---

## 6. Testes de Validação e Qualidade

Para garantir a estabilidade e o encapsulamento, a equipe construiu uma bateria dupla de validações:

### 6.1. Testes Unitários de Software (`src/cpu/test/`)
Testes de caixa branca em C++ que atestam a matemática e a lógica isolada de cada módulo antes de integra-los:
* `test_ula.cpp`: Valida operações aritméticas, limites, lógicas booleanas e detecção de *overflow*.
* `test_control_unit.cpp`: Verifica o correto mascaramento de bits (*Decode*) e as delegações de tarefas.
* `test_registers.cpp`: Confirma a estrutura do Banco de Registradores e simula o bloqueio de I/O de memória indevida.
* `test_cpu_integration.cpp`: Um teste estrutural da CPU montada conectada com a UC, ULA e Registradores.
* `test_cpu_standalone.cpp`: Script especial usado para invocar e depurar a máquina isoladamente, simulando um "Falso SO" na Memória.

*(Para rodar todos simultaneamente, basta executar `make test` na raiz do repositório).*

### 6.2. Cargas de Teste / Jobs de Aceitação (`src/cpu/jobs/`)
Arquivos de texto baseados em nosso Assembly (instruções puramente em Hexadecimal) criados para a equipe de SO fornecer ao seu *Loader*. Eles servem para atestar que o processador domina o controle de fluxo na vida real:
* **`soma_simples.txt`**: Testa o fluxo básico de `LOAD`, `ADD`, `STORE` e `HALT`.
* **`operacoes_basicas.txt`**: Testa as operações matemáticas e lógicas e a estabilidade da ULA.
* **`teste_logico_or.txt`**: Valida a operação de mascaramento lógico bit a bit (`OR`).
* **`teste_salto_jump.txt`**: Valida a precisão da alteração do *Program Counter* em desvios incondicionais.
