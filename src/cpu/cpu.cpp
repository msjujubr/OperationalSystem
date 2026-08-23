#include "cpu.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>

// UNIDADE CENTRAL DE PROCESSAMENTO (CPU)
CPU::CPU() {
    reset();
}

void CPU::reset() {
    regBank.reset();
    regBank.setPC(OS_RESERVED_MEM);   // Inicia após a área do SO
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

// ----- ULA (Unidade Lógica e Aritmética) --------------------------------------
void CPU::ULA(uint16_t opcode, uint16_t regDest, uint16_t regF1, uint16_t regF2) {
    uint16_t val1 = regBank.read(regF1);
    uint16_t val2 = regBank.read(regF2);
    uint16_t res = 0;

    switch (opcode) {
        case OP_ADD: res = val1 + val2; break;
        case OP_SUB: res = val1 - val2; break;
        case OP_AND: res = val1 & val2; break;
        case OP_OR:  res = val1 | val2; break;
        default:
            throw std::runtime_error("ULA: Opcode invalido!");
    }

    regBank.write(regDest, res);
}

// ----- Unidade de Controle (Busca → Decodificação → Execução) ----------------
void CPU::step() {
    if (haltStatus) return;

    // 1. BUSCA
    uint16_t pc = regBank.getPC();
    uint16_t IR = LerMemoria(pc);
    regBank.setIR(IR);
    regBank.incrementPC();   // Avança PC

    // 2. DECODIFICAÇÃO (Mascaramento de bits)
    uint16_t opcode    = (IR >> 12) & 0x000F;
    uint16_t regDest   = (IR >> 8)  & 0x000F;
    uint16_t regF1     = (IR >> 4)  & 0x000F;
    uint16_t regF2     =  IR        & 0x000F;
    uint16_t endereco  =  IR        & 0x00FF;   // Usado para LOAD/STORE/BEQ

    // 3. EXECUÇÃO
    switch (opcode) {
        case OP_LOAD:
            regBank.write(regDest, LerMemoria(endereco + OS_RESERVED_MEM));
            break;

        case OP_STORE:
            EscreverMemoria(endereco + OS_RESERVED_MEM, regBank.read(regDest));
            break;

        case OP_ADD:
        case OP_SUB:
        case OP_AND:
        case OP_OR:
            ULA(opcode, regDest, regF1, regF2);
            break;

        case OP_BEQ:
            if (regBank.read(regDest) == regBank.read(regF1)) {
                regBank.setPC(endereco + OS_RESERVED_MEM);
            }
            break;

        case OP_JUMP:
            regBank.setPC((IR & 0x0FFF) + OS_RESERVED_MEM);
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
void CPU::imprimirEstado(std::ostream& out) const {
    regBank.imprimirEstado(out);
}
