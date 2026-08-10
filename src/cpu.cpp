#include "cpu.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>

// UNIDADE CENTRAL DE PROCESSAMENTO (CPU)
CPU::CPU() {
    reset();
}

void CPU::reset() {
    for (int i = 0; i < 8; i++) R[i] = 0;
    PC = OS_RESERVED_MEM;   // Inicia após a área do SO
    IR = 0;
    haltStatus = false;
}

void CPU::setPC(uint16_t startAddress) {
    PC = startAddress;
}

bool CPU::isHalted() const {
    return haltStatus;
}

// ----- ULA (Unidade Lógica e Aritmética) --------------------------------------
void CPU::ULA(uint16_t opcode, uint16_t regDest, uint16_t regF1, uint16_t regF2) {
    switch (opcode) {
        case OP_ADD: R[regDest] = R[regF1] + R[regF2]; break;
        case OP_SUB: R[regDest] = R[regF1] - R[regF2]; break;
        case OP_AND: R[regDest] = R[regF1] & R[regF2]; break;
        case OP_OR:  R[regDest] = R[regF1] | R[regF2]; break;
        default:
            // Não deve ocorrer, pois o chamador já validou o opcode
            throw std::runtime_error("ULA: Opcode invalido!");
    }
}

// ----- Unidade de Controle (Busca → Decodificação → Execução) ----------------
void CPU::step() {
    if (haltStatus) return;

    // 1. BUSCA
    IR = LerMemoria(PC);
    PC++;   // Avança PC

    // 2. DECODIFICAÇÃO (Mascaramento de bits)
    uint16_t opcode    = (IR >> 12) & 0x000F;
    uint16_t regDest   = (IR >> 8)  & 0x000F;
    uint16_t regF1     = (IR >> 4)  & 0x000F;
    uint16_t regF2     =  IR        & 0x000F;
    uint16_t endereco  =  IR        & 0x00FF;   // Usado para LOAD/STORE/BEQ

    // 3. EXECUÇÃO
    switch (opcode) {
        case OP_LOAD:
            R[regDest] = LerMemoria(endereco + OS_RESERVED_MEM);
            break;

        case OP_STORE:
            EscreverMemoria(endereco + OS_RESERVED_MEM, R[regDest]);
            break;

        case OP_ADD:
        case OP_SUB:
        case OP_AND:
        case OP_OR:
            ULA(opcode, regDest, regF1, regF2);
            break;

        case OP_BEQ:
            if (R[regDest] == R[regF1]) {
                PC = (endereco + OS_RESERVED_MEM);
            }
            break;

        case OP_JUMP:
            PC = ((IR & 0x0FFF) + OS_RESERVED_MEM);
            break;

        case OP_HALT:
            haltStatus = true;
            break;

        default:
            throw std::runtime_error("Opcode Invalido detectado!");
    }

    instExecutadas++;
}

// ----- Impressão do estado da CPU --------------------------------------------
void CPU::imprimirEstado() const {
    std::cout << "--- ESTADO DA CPU ---" << std::endl;
    std::cout << "PC: 0x" << std::setfill('0') << std::setw(4) << std::hex << PC
              << "   IR: 0x" << std::setw(4) << IR << std::dec << std::endl;
    std::cout << "Registradores Gerais:" << std::endl;
    for (int i = 0; i < 8; i += 4) {
        std::cout << "R" << i << ": 0x" << std::setfill('0') << std::setw(4) << std::hex << R[i] << "  "
                  << "R" << i+1 << ": 0x" << std::setw(4) << R[i+1] << "  "
                  << "R" << i+2 << ": 0x" << std::setw(4) << R[i+2] << "  "
                  << "R" << i+3 << ": 0x" << std::setw(4) << R[i+3] << std::dec << std::endl;
    }
}