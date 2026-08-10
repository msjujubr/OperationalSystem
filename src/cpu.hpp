#ifndef CPU_HPP
#define CPU_HPP
#include "memory.hpp"
#include <cstdint>

// UNIDADE CENTRAL DE PROCESSAMENTO (CPU)
class CPU {
private:
    uint16_t R[8];          // 8 Registradores de uso geral
    uint16_t PC;            // Program Counter
    uint16_t IR;            // Instruction Register
    bool haltStatus;

    /**
     * ULA - Unidade Lógica e Aritmética (sem acesso à memória)
     * @param opcode  Código da operação (ADD, SUB, AND, OR)
     * @param regDest Registrador de destino
     * @param regF1   Primeiro operando (registrador fonte)
     * @param regF2   Segundo operando (registrador fonte)
     */
    void ULA(uint16_t opcode, uint16_t regDest, uint16_t regF1, uint16_t regF2);

public:
    CPU();

    /** Reinicia a CPU (zera registradores e aponta PC para a área do SO) */
    void reset();

    /** Define o valor do Program Counter */
    void setPC(uint16_t startAddress);

    /** Retorna true se a CPU está em estado HALT */
    bool isHalted() const;

    /**
     * step - Ciclo de Busca, Decodificação e Execução (Unidade de Controle)
     * @throws std::runtime_error se opcode for inválido
     */
    void step();

    /** Exibe o estado atual da CPU (PC, IR e registradores) */
    void imprimirEstado() const;
};

#endif // CPU_HPP