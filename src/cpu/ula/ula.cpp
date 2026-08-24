#include "ula.hpp"
#include <limits>
#include <iostream>

void ULA::calculate() {
    // Reset de flags e resultado antes de cada cálculo
    overflow = false;
    result = 0;

    // Lambda para conversão segura de uint16_t para int16_t (signed)
    auto as_i16 = [](uint16_t x) -> int16_t {
        return static_cast<int16_t>(x);
    };

    // Lambda para checagem de overflow (verifica se cabe no intervalo [-32768, 32767])
    auto fits_in_i16 = [](int32_t x) -> bool {
        return (x <= std::numeric_limits<int16_t>::max()) &&
               (x >= std::numeric_limits<int16_t>::min());
    };

    switch (op) {
        case ADD: {
            // Soma com cálculo intermediário em 32 bits para detecção de overflow
            int32_t res32 = static_cast<int32_t>(as_i16(A)) + static_cast<int32_t>(as_i16(B));
            if (!fits_in_i16(res32)) {
                overflow = true;
            }
            result = static_cast<int16_t>(static_cast<uint16_t>(res32 & 0xFFFF));
            break;
        }

        case SUB: {
            // Subtração (A - B) com detecção de overflow
            int32_t res32 = static_cast<int32_t>(as_i16(A)) - static_cast<int32_t>(as_i16(B));
            if (!fits_in_i16(res32)) {
                overflow = true;
            }
            result = static_cast<int16_t>(static_cast<uint16_t>(res32 & 0xFFFF));
            break;
        }

        case MUL: {
            // Multiplicação (A * B) com detecção de overflow
            int32_t res32 = static_cast<int32_t>(as_i16(A)) * static_cast<int32_t>(as_i16(B));
            if (!fits_in_i16(res32)) {
                overflow = true;
            }
            result = static_cast<int16_t>(static_cast<uint16_t>(res32 & 0xFFFF));
            break;
        }

        case DIV: {
            // Divisão inteira (A / B) com tratamento de divisão por zero e overflow
            int16_t divisor = as_i16(B);
            if (divisor == 0) {
                overflow = true;
                result = 0;
            } else {
                int16_t dividendo = as_i16(A);
                // Overflow crítico de complemento de dois: INT16_MIN / -1 = 32768 (estoura int16)
                if (dividendo == std::numeric_limits<int16_t>::min() && divisor == -1) {
                    overflow = true;
                    result = std::numeric_limits<int16_t>::min();
                } else {
                    result = dividendo / divisor;
                }
            }
            break;
        }

        case AND_OP: {
            // Operação lógica bit a bit AND (valores unsigned)
            result = static_cast<int16_t>(A & B);
            break;
        }

        case OR_OP: {
            // Operação lógica bit a bit OR (valores unsigned)
            result = static_cast<int16_t>(A | B);
            break;
        }

        case BEQ: {
            // Branch if Equal: retorna 1 se A == B, senão 0
            result = (as_i16(A) == as_i16(B)) ? 1 : 0;
            break;
        }

        case BNE: {
            // Branch if Not Equal: retorna 1 se A != B, senão 0
            result = (as_i16(A) != as_i16(B)) ? 1 : 0;
            break;
        }

        case BLT: {
            // Branch if Less Than: retorna 1 se A < B, senão 0
            result = (as_i16(A) < as_i16(B)) ? 1 : 0;
            break;
        }

        case BGT: {
            // Branch if Greater Than: retorna 1 se A > B, senão 0
            result = (as_i16(A) > as_i16(B)) ? 1 : 0;
            break;
        }

        case JUMP: {
            // Salto incondicional: retorna sempre 1
            result = 1;
            break;
        }

        case BGTI: {
            // Branch if Greater Than Immediate (A > B)
            result = (as_i16(A) > as_i16(B)) ? 1 : 0;
            break;
        }

        case BLTI: {
            // Branch if Less Than Immediate (A < B)
            result = (as_i16(A) < as_i16(B)) ? 1 : 0;
            break;
        }

        case LW:
        case LA:
        case ST: {
            // Cálculo de endereço efetivo: base + offset
            int32_t addr32 = static_cast<int32_t>(as_i16(A)) + static_cast<int32_t>(as_i16(B));
            if (!fits_in_i16(addr32)) {
                overflow = true;
            }
            result = static_cast<int16_t>(static_cast<uint16_t>(addr32 & 0xFFFF));
            break;
        }

        case HALT: {
            result = 0;
            break;
        }

        default: {
            overflow = true;
            result = 0;
            break;
        }
    }
}

void ULA::execute(operation ULA_operacao, uint16_t a, uint16_t b, uint16_t /*shamt*/) {
    op = ULA_operacao;
    A = a;
    B = b;
    calculate();
}

bool ULA::executeOpcode(uint16_t opcode, uint16_t a, uint16_t b) {
    switch (opcode) {
        case OP_ADD:
            execute(ADD, a, b);
            return true;
        case OP_SUB:
            execute(SUB, a, b);
            return true;
        case OP_AND:
            execute(AND_OP, a, b);
            return true;
        case OP_OR:
            execute(OR_OP, a, b);
            return true;
        case OP_BEQ:
            execute(BEQ, a, b);
            return true;
        default:
            return false;
    }
}
