#ifndef CPU_HPP
#define CPU_HPP

#include "registers/register_bank.hpp"
#include "ula/ula.hpp"
#include "../memory.hpp"
#include "../defines.hpp"
#include <cstdint>
#include <iostream>
#include <ostream>

/**
 * ============================================================================
 * @brief Unidade Central de Processamento (CPU)
 * ============================================================================
 * Atua como o núcleo integrador da arquitetura computacional simulada.
 * 
 * Responsabilidades:
 * 1. Integração Modular: Coordena o Banco de Registradores (RegisterBank) e a ULA.
 * 2. Ciclo de Instrução (Busca -> Decodificação -> Execução).
 * 3. Barramento de Memória: Realiza transferências Load/Store via interface de memória.
 */
class CPU {
private:
    RegisterBank regBank;   ///< Banco de Registradores modularizado (R0..R7, PC, IR)
    ULA ula;                ///< Unidade Lógica e Aritmética modularizada
    bool haltStatus;        ///< Flag que indica se a CPU está parada (HALT)

    /**
     * @brief Despacha uma operação computacional para a ULA e salva o resultado no registrador.
     * @param opcode  Código da operação (OP_ADD, OP_SUB, OP_AND, OP_OR, etc.).
     * @param regDest Índice do registrador de destino (0..7).
     * @param regF1   Índice do primeiro registrador fonte (0..7).
     * @param regF2   Índice do segundo registrador fonte (0..7).
     */
    void dispararULA(uint16_t opcode, uint16_t regDest, uint16_t regF1, uint16_t regF2);

public:
    CPU();

    /** Reinicia a CPU (zera registradores e aponta PC para a área inicial do job) */
    void reset();

    /** Define o valor do Program Counter */
    void setPC(uint16_t startAddress);

    /** Retorna true se a CPU está em estado HALT */
    bool isHalted() const;

    /**
     * @brief step - Executa um ciclo completo de instrução (Busca, Decodificação, Execução).
     * @throws std::runtime_error se um opcode desconhecido for encontrado.
     */
    void step();

    /** Exibe o estado atual da CPU (PC, IR e registradores) em qualquer stream */
    void imprimirEstado(std::ostream& out = std::cout) const;

    /** Acesso ao Banco de Registradores */
    RegisterBank& getRegisterBank();
    const RegisterBank& getRegisterBank() const;

    /** Acesso à ULA */
    ULA& getULA();
    const ULA& getULA() const;
};

#endif // CPU_HPP

