#include "cpu.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>

// ----------------------------------------------------------------------------
// Construtor e Controle Básico da CPU
// ----------------------------------------------------------------------------
CPU::CPU() {
    reset();
}

void CPU::reset() {
    regBank.reset();
    regBank.setPC(OS_RESERVED_MEM); // Inicia apontando para o início da área do job na RAM
    haltStatus = false;
}

void CPU::setPC(uint16_t startAddress) {
    regBank.setPC(startAddress);
}

bool CPU::isHalted() const {
    return haltStatus;
}

RegisterBank& CPU::getRegisterBank() {
    return regBank;
}

const RegisterBank& CPU::getRegisterBank() const {
    return regBank;
}

ULA& CPU::getULA() {
    return ula;
}

const ULA& CPU::getULA() const {
    return ula;
}

// ----------------------------------------------------------------------------
// Despacho para a ULA (Unidade Lógica e Aritmética)
// ----------------------------------------------------------------------------
void CPU::dispararULA(uint16_t opcode, uint16_t regDest, uint16_t regF1, uint16_t regF2) {
    // 1. Leitura dos operandos diretamente do Banco de Registradores
    uint16_t val1 = regBank.read(regF1);
    uint16_t val2 = regBank.read(regF2);

    // 2. Execução da operação na ULA
    if (!ula.executeOpcode(opcode, val1, val2)) {
        throw std::runtime_error("CPU::dispararULA - Opcode invalido ou nao computacional: " + std::to_string(opcode));
    }

    // 3. Escrita do resultado de 16 bits de volta no Banco de Registradores
    regBank.write(regDest, static_cast<uint16_t>(ula.result));
}

// ----------------------------------------------------------------------------
// Ciclo de Instrução (Busca -> Decodificação -> Execução)
// ----------------------------------------------------------------------------
void CPU::step() {
    if (haltStatus) return;

    // 1. BUSCA (Instruction Fetch)
    // Lê a instrução de 16 bits apontada pelo PC na memória RAM e incrementa PC
    uint16_t pc = regBank.getPC();
    uint16_t IR = LerMemoria(pc);
    regBank.setIR(IR);
    regBank.incrementPC();

    // 2. DECODIFICAÇÃO (Instruction Decode)
    // Extração dos campos da instrução através de operações de mascaramento de bits
    uint16_t opcode    = (IR >> 12) & 0x000F; // Bits [15:12] -> Código da Operação
    uint16_t regDest   = (IR >> 8)  & 0x000F; // Bits [11:8]  -> Registrador de Destino (ou Reg1)
    uint16_t regF1     = (IR >> 4)  & 0x000F; // Bits [7:4]   -> Primeiro Registrador Fonte
    uint16_t regF2     =  IR        & 0x000F; // Bits [3:0]   -> Segundo Registrador Fonte
    uint16_t endereco  =  IR        & 0x00FF; // Bits [7:0]   -> Endereço imediato (Tipo I)

    // 3. EXECUÇÃO (Instruction Execute)
    switch (opcode) {
        // --- Operações de Transferência de Memória (Paradigma Load/Store) ---
        case OP_LOAD:
            // Traz dado da RAM para o registrador de destino
            regBank.write(regDest, LerMemoria(endereco + OS_RESERVED_MEM));
            break;

        case OP_STORE:
            // Salva dado do registrador de destino na RAM
            EscreverMemoria(endereco + OS_RESERVED_MEM, regBank.read(regDest));
            break;

        // --- Operações Aritméticas e Lógicas (Delegadas à ULA) ---
        case OP_ADD:
        case OP_SUB:
        case OP_AND:
        case OP_OR:
            dispararULA(opcode, regDest, regF1, regF2);
            break;

        // --- Operações de Controle de Fluxo ---
        case OP_BEQ:
            // Compara os dois registradores via ULA
            ula.execute(BEQ, regBank.read(regDest), regBank.read(regF1));
            if (ula.result == 1) {
                regBank.setPC(endereco + OS_RESERVED_MEM);
            }
            break;

        case OP_JUMP:
            // Salto incondicional: atualiza PC imediatamente
            regBank.setPC((IR & 0x0FFF) + OS_RESERVED_MEM);
            break;

        // --- Instrução de Sistema / Término ---
        case OP_HALT:
            haltStatus = true;
            break;

        default:
            throw std::runtime_error("CPU::step - Opcode Invalido detectado: 0x" + std::to_string(opcode));
    }

    instExecutadas++;
}

// ----------------------------------------------------------------------------
// Impressão do Estado da CPU
// ----------------------------------------------------------------------------
void CPU::imprimirEstado(std::ostream& out) const {
    regBank.imprimirEstado(out);
}

