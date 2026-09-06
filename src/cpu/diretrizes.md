# Diretrizes de Arquitetura e Engenharia: Subsistema de CPU

Este documento define as convenções arquiteturais, padrões de código e diretrizes de integração para todos os integrantes que colaboram no desenvolvimento do subsistema de **CPU** da disciplina de Sistemas Operacionais.

---

## 1. Organização e Estrutura de Pastas

Para garantir a modularidade, isolamento de responsabilidades e evitar conflitos de *merge* no Git, todos os componentes da CPU devem seguir rigorosamente a hierarquia abaixo:

```text
src/
├── common/ (ou raiz de src/)
│   └── defines.hpp             # Definições globais compartilhadas (Opcodes, constantes)
│
├── cpu/                        # Diretório raiz do subsistema de CPU
│   ├── diretrizes.md           # Este documento de referência
│   ├── cpu.hpp / cpu.cpp       # Classe principal integradora da CPU
│   │
│   ├── registers/              # [Sub-equipe de Registradores]
│   │   ├── register_bank.hpp / .cpp
│   │   └── register_table.hpp / .cpp
│   │
│   ├── ula/                    # [Sub-equipe da ULA]
│   │   ├── ula.hpp / .cpp
│   │
│   └── control/                # [Sub-equipe da Unidade de Controle]
│       ├── control_unit.hpp / .cpp
│       └── instruction_decoder.hpp / .cpp (se aplicável)
│
├── memory/                     # [Sub-equipe de Memória - RAM, Cache, Disco]
└── os/                         # [Sub-equipe de SO - PCB, Escalonador, Batch]
```

---

## 2. Tipagem e Regras de 16 Bits

A arquitetura oficial do simulador (conforme `Pratica0.pdf`) é uma máquina **RISC de 16 bits**:

1. **Palavra de Dados e Endereços:** Use sempre `uint16_t` (da biblioteca `<cstdint>`) para barramentos de dados, registradores e endereços de memória.
2. **Aritmética com Sinal:** Para operações que exigem sinal na ULA (como `SUB`, `MUL`, `DIV` ou comparações `BEQ`/`BLT`), faça a conversão explícita para `int16_t` e use tipos intermediários de 32 bits (`int32_t`) para verificar **overflow**.
3. **Indexação:** Use `size_t` para índices de arrays/vetores e loops internos para evitar problemas de conversão com sinal ou *narrowing conversions*.

---

## 3. Uso do `defines.hpp` Centralizado (Sem Enums Duplicados)

Para que a **Unidade de Controle**, a **ULA** e o **Montador** falem a mesma língua sem tradutores intermediários, **todas as constantes e códigos de operação devem vir exclusivamente de `defines.hpp`**:

```cpp
// src/defines.hpp
enum Opcodes {
    OP_LOAD  = 0x1, // Traz dado da Memória -> Registrador
    OP_ADD   = 0x2, // Soma
    OP_STORE = 0x3, // Salva Registrador -> Memória
    OP_SUB   = 0x4, // Subtração
    OP_AND   = 0x5, // Lógica AND
    OP_OR    = 0x6, // Lógica OR
    OP_BEQ   = 0x7, // Branch if Equal
    OP_JUMP  = 0x8, // Salto incondicional
    OP_HALT  = 0xF  // Fim do job
};

const uint32_t TAM_RAM         = 65536;
const uint16_t OS_RESERVED_MEM = 512;
```

> ⚠️ **Atenção:** Evite criar `enum operation` ou enums locais dentro dos arquivos de cada módulo. Utilize os identificadores `OP_*` do `defines.hpp`.

---

## 4. Convenções do Banco de Registradores (`RegisterBank`)

1. **Quantidade e Nomes:** A CPU possui exatamente **8 registradores de uso geral** (`R0` a `R7`), além de dois registradores de controle: **`PC`** (Program Counter) e **`IR`** (Instruction Register).
2. **Comportamento do `R0`:** `R0` é um registrador de leitura e escrita normal (não é travado em zero como no MIPS clássico).
3. **Interface para a CPU:**
   * Leitura: `uint16_t regBank.read(size_t index);`
   * Escrita: `void regBank.write(size_t index, uint16_t value);`
   * PC / IR: `regBank.getPC()`, `regBank.setPC(val)`, `regBank.incrementPC()`, `regBank.getIR()`, `regBank.setIR(val)`.
4. **Integração com o PCB (SO):**
   * O `RegisterBank` disponibiliza `CPUContext getContext()` e `void setContext(const CPUContext&)` para salvar e restaurar o estado completo da CPU (`R0..R7`, `PC`, `IR`) durante trocas de contexto.

---

## 5. Fronteiras Arquiteturais e Paradigma Load/Store

1. **Isolamento da ULA:**
   * A ULA **nunca acessa a memória RAM diretamente**. Ela apenas recebe operandos (`A` e `B`) e devolve o resultado (`result` e `overflow`).
2. **Responsabilidade da ULA vs Unidade de Controle:**
   * Operações de fluxo (`JUMP`) e de parada (`HALT`) são tratadas **diretamente pela Unidade de Controle**, alterando o `PC` ou a flag `haltStatus`. A ULA não deve implementar lógica para `JUMP` ou `HALT`.
3. **Transferência de Memória:**
   * Toda transferência entre RAM e CPU ocorre exclusivamente através das instruções `LOAD` e `STORE`, mediadas pela função `LerMemoria` / `EscreverMemoria`.

---

## 6. Padrões de I/O e Relatório de Saída (`output.dat`)

Conforme exigido na especificação da Prática 0, a CPU precisa exibir o estado final no terminal e salvar no arquivo `output.dat`.

* Métodos de impressão de estado devem aceitar um fluxo genérico:
  ```cpp
  void imprimirEstado(std::ostream& out = std::cout) const;
  ```
* Valores hexadecimais devem ser formatados com **4 dígitos, zeros à esquerda e letras maiúsculas** (`std::uppercase`):
  ```text
  --- ESTADO DA CPU ---
  PC: 0x0205   IR: 0xF000
  Registradores Gerais:
  R0: 0x0005  R1: 0x0007  R2: 0x000C  R3: 0x0000
  R4: 0x0000  R5: 0x0000  R6: 0x0000  R7: 0x0000
  ```

---

## 7. Testes Automatizados

Cada módulo (`registers`, `ula`, `control`) deve possuir seus próprios testes unitários dentro do diretório `tests/` (ex: `tests/test_registers.cpp`, `tests/test_ula.cpp`), garantindo que o módulo funcione de forma isolada antes da integração final.
