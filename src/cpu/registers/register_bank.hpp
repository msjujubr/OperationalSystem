#ifndef REGISTER_BANK_HPP
#define REGISTER_BANK_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <ostream>

/**
 * @brief Estrutura completa de contexto da CPU para o Process Control Block (PCB).
 * Utilizada pelo Sistema Operacional para troca de contexto e multiprogramação.
 */
struct CPUContext {
    std::array<uint16_t, 8> R;
    uint16_t PC;
    uint16_t IR;
};

/**
 * @brief Banco de Registradores da CPU RISC (16-bits)
 * 
 * Responsável por armazenar o estado dos 8 registradores de uso geral (R0 a R7)
 * e dos registradores especiais de controle (PC e IR).
 */
class RegisterBank {
public:
    static constexpr size_t NUM_REGISTERS = 8;

private:
    std::array<uint16_t, NUM_REGISTERS> R; // Registradores de uso geral R0..R7
    uint16_t PC;                           // Program Counter
    uint16_t IR;                           // Instruction Register

public:
    RegisterBank();

    /**
     * @brief Lê o valor de um registrador de uso geral.
     * @param index Índice do registrador (0 a NUM_REGISTERS - 1).
     * @return Valor armazenado de 16 bits.
     * @throws std::out_of_range se index >= NUM_REGISTERS.
     */
    uint16_t read(size_t index) const;

    /**
     * @brief Escreve um valor em um registrador de uso geral.
     * @param index Índice do registrador (0 a NUM_REGISTERS - 1).
     * @param value Valor de 16 bits a ser escrito.
     * @throws std::out_of_range se index >= NUM_REGISTERS.
     */
    void write(size_t index, uint16_t value);

    // --- Registradores Especiais ---

    uint16_t getPC() const noexcept;
    void setPC(uint16_t address) noexcept;
    void incrementPC() noexcept;

    uint16_t getIR() const noexcept;
    void setIR(uint16_t instruction) noexcept;

    // --- Métodos de Controle e Contexto (PCB / SO) ---

    /** Reinicia todos os registradores para zero (troca de job em lote) */
    void reset() noexcept;

    /** Retorna o contexto completo da CPU (R0..R7, PC, IR) para o PCB */
    CPUContext getContext() const noexcept;

    /** Restaura o contexto completo da CPU a partir do PCB */
    void setContext(const CPUContext& ctx) noexcept;

    /** Exporta snapshot de R0..R7 em vetor (compatibilidade) */
    std::vector<uint16_t> getRegistersSnapshot() const;

    /** Restaura snapshot de R0..R7 a partir de vetor */
    void loadRegistersSnapshot(const std::vector<uint16_t>& snapshot);

    /**
     * @brief Exibe o estado formatado da CPU em qualquer stream (console ou output.dat).
     * @param out Fluxo de saída (padrão: std::cout).
     */
    void imprimirEstado(std::ostream& out = std::cout) const;
};

#endif // REGISTER_BANK_HPP
