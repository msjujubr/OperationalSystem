#include "register_bank.hpp"
#include <iomanip>
#include <stdexcept>

RegisterBank::RegisterBank() {
    reset();
}

void RegisterBank::reset() noexcept {
    R.fill(0);
    PC = 0;
    IR = 0;
}

uint16_t RegisterBank::read(size_t index) const {
    if (index >= NUM_REGISTERS) {
        throw std::out_of_range("RegisterBank::read - Indice invalido (" + 
                                std::to_string(index) + "). Permitido: 0 a " + 
                                std::to_string(NUM_REGISTERS - 1) + ".");
    }
    return R[index];
}

void RegisterBank::write(size_t index, uint16_t value) {
    if (index >= NUM_REGISTERS) {
        throw std::out_of_range("RegisterBank::write - Indice invalido (" + 
                                std::to_string(index) + "). Permitido: 0 a " + 
                                std::to_string(NUM_REGISTERS - 1) + ".");
    }
    R[index] = value;
}

uint16_t RegisterBank::getPC() const noexcept {
    return PC;
}

void RegisterBank::setPC(uint16_t address) noexcept {
    PC = address;
}

void RegisterBank::incrementPC() noexcept {
    PC++;
}

uint16_t RegisterBank::getIR() const noexcept {
    return IR;
}

void RegisterBank::setIR(uint16_t instruction) noexcept {
    IR = instruction;
}

CPUContext RegisterBank::getContext() const noexcept {
    CPUContext ctx;
    ctx.R = R;
    ctx.PC = PC;
    ctx.IR = IR;
    return ctx;
}

void RegisterBank::setContext(const CPUContext& ctx) noexcept {
    R = ctx.R;
    PC = ctx.PC;
    IR = ctx.IR;
}

std::vector<uint16_t> RegisterBank::getRegistersSnapshot() const {
    return std::vector<uint16_t>(R.begin(), R.end());
}

void RegisterBank::loadRegistersSnapshot(const std::vector<uint16_t>& snapshot) {
    if (snapshot.size() != NUM_REGISTERS) {
        throw std::invalid_argument("RegisterBank::loadRegistersSnapshot - Snapshot deve conter exatamente " + 
                                    std::to_string(NUM_REGISTERS) + " registradores.");
    }
    for (size_t i = 0; i < NUM_REGISTERS; ++i) {
        R[i] = snapshot[i];
    }
}

void RegisterBank::imprimirEstado(std::ostream& out) const {
    // Salva o estado dos formatadores do stream para não causar efeitos colaterais
    std::ios state(nullptr);
    state.copyfmt(out);

    out << "--- ESTADO DA CPU ---" << std::endl;
    out << "PC: 0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << PC
        << "   IR: 0x" << std::setw(4) << IR << std::nouppercase << std::dec << std::endl;
    out << "Registradores Gerais:" << std::endl;
    for (size_t i = 0; i < NUM_REGISTERS; i += 4) {
        out << "R" << i << ": 0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << R[i] << "  "
            << "R" << i+1 << ": 0x" << std::setw(4) << R[i+1] << "  "
            << "R" << i+2 << ": 0x" << std::setw(4) << R[i+2] << "  "
            << "R" << i+3 << ": 0x" << std::setw(4) << R[i+3] << std::nouppercase << std::dec << std::endl;
    }

    out.copyfmt(state);
}
