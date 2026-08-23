#include "ULA.hpp"
#include <limits> // oferece os limites dos tipos numéricos
#include <iostream>
/*
  Observações:
  - Terminologia: Base = endereço inicial ; Offset = deslocamento
  - Todos os cálculos são realizados interpretando A e B como inteiros com sinal
    de 16 bits (int16_t) quando aplicável (operações aritméticas e comparações).
  - Para detectar overflow aritmético usei aritmética de 32 bits (int32_t)
    como intermediário e chequei se o resultado cabe em 16 bits.
  - Para operações de comparação (BEQ/BNE/BLT/BGT e variantes com imediato),
    o campo `result` assume 1 quando a condição é verdadeira e 0 caso contrário.
  - Para operações de memória (LW, LA, ST) a ULA retorna o endereço efetivo
    calculado normalmente como base + offset.
*/

void ULA::calculate(){

    // reset flags/resultado antes de cada execução
    overflow = false;
    result = 0;

    // converte números sem sinal para números com sinal
    auto as_i16 = [](uint16_t x) -> int16_t
    { 
        return static_cast<int16_t>(x); 
    };
    // verifica overflow, ou seja, se o resultado da operação cabe em 16 bits com sinal
    auto fits_in_i16 = [](int32_t x) -> bool
    {
        return (x <= std::numeric_limits<int16_t>::max()) &&
               (x >= std::numeric_limits<int16_t>::min());
    };

    switch (op)
    {
    case ADD:
    {
        // Soma com detecção de overflow (signed) - 16 bits.  Aqui, usamos o static cast 32 bits árar converter A e B sem que estoure. 
        int32_t res32 = static_cast<int32_t>(as_i16(A)) + static_cast<int32_t>(as_i16(B));
        if (!fits_in_i16(res32))
        {
            overflow = true;
        }
        // Resultado final em 16 bits (armazenado como uint16_t)
        result = static_cast<uint16_t>(res32);
        //std::cout << result << std::endl;
        break;
    }

    case SUB:
    {
        // Subtração com detecção de overflow (signed) - 16 bits
        int32_t res32 = static_cast<int32_t>(as_i16(A)) - static_cast<int32_t>(as_i16(B));
        if (!fits_in_i16(res32))
        {
            overflow = true;
        }
        result = static_cast<uint16_t>(res32);
        break;
    }

    case MUL:
    {
        // Multiplicação com detecção de overflow (signed) - 16 bits
        // 16x16 = 32 bits máximo
        int32_t res32 = static_cast<int32_t>(as_i16(A)) * static_cast<int32_t>(as_i16(B));
        if (!fits_in_i16(res32))
        {
            overflow = true;
        }
        result = static_cast<uint16_t>(res32);
        break;
    }

    case DIV:
    {
        // Divisão (signed) - 16 bits. Tratar divisão por zero.
        int16_t divisor = as_i16(B);
        if (divisor == 0)
        {
            // Convenção: sinaliza overflow/erro de divisão por zero
            overflow = true;
            result = 0;
        }
        else
        {
            // Cuidado com INT16_MIN / -1 -> overflow (porque |INT16_MIN| > INT16_MAX)
            int16_t dividendo = as_i16(A);
            if (dividendo == std::numeric_limits<int16_t>::min() && divisor == -1)
            {
                overflow = true;
                // Resultado matematicamente é 32768, mas não cabe em int16
                result = static_cast<uint16_t>(std::numeric_limits<int16_t>::min());
            }
            else
            {
                int16_t resultado_div = dividendo / divisor;
                result = static_cast<uint16_t>(resultado_div);
            }
        }
        break;
    }

    case AND_OP:
    {
        // Operação bit a bit AND (interpretamos A e B como unsigned)
        result = A & B;
        break;
    }

    case OR_OP:
    {
        // Operação bit a bit OR (interpretamos A e B como unsigned)
        result = A | B;  // O mesmo que A OR B
        break;
    }
    case BEQ:
    {
        // Branch if equal: result = 1 se A == B, senão 0 (comparação signed)
        result = (as_i16(A) == as_i16(B)) ? 1 : 0;
        break;
    }

    case BNE:
    {
        // Branch if not equal (comparação signed)
        result = (as_i16(A) != as_i16(B)) ? 1 : 0;
        break;
    }

    case BLT:
    {
        // Branch if less than (signed)
        result = (as_i16(A) < as_i16(B)) ? 1 : 0;
        break;
    }

    case BGT:
    {
        // Branch if greater than (signed)
        result = (as_i16(A) > as_i16(B)) ? 1 : 0;
        break;
    }

    case JUMP:
    {
        // Jump incondicional: sempre retorna 1 (verdadeiro)
        // O processador vai interpretar isso como "desvie sempre"
        result = 1;
        break;
    }

    case BGTI:
    {
        // Branch if greater than immediate
        // Convenção: B contém o imediato; compara A > B (signed)
        result = (as_i16(A) > as_i16(B)) ? 1 : 0;
        break;
    }

    case BLTI:
    {
        // Branch if less than immediate
        // Convenção: B contém o imediato; compara A < B (signed)
        result = (as_i16(A) < as_i16(B)) ? 1 : 0;
        break;
    }

    case LW:
    case LA:
    case ST:
    {
        // Operações de memória: calculam endereço efetivo = base + offset
        // (A = base, B = offset/imediato). Usa aritmética signed para
        // permitir offsets negativos.
        int32_t addr32 = static_cast<int32_t>(as_i16(A)) + static_cast<int32_t>(as_i16(B));
        if (!fits_in_i16(addr32))
        {
            overflow = true;
        }
        result = static_cast<uint16_t>(addr32);
        break;
    }

    case HALT:
    {
    // HALT: sinaliza para o processador parar
    // A ULA não faz nada, apenas retorna um valor especial
    result = 0;
    // O processador que deve interpretar isso como "pare"
    break;
    }

    default:
    {
        // Caso não esperado: definir resultado 0 e sinalizar overflow = true
        overflow = true;
        result = 0;
        break;
    }
    } 
} 

void ULA::execute(operation ULA_operacao, uint16_t a, uint16_t b, uint16_t shamt){
    // Carregando as entradas. A e B são números decimais que serão convertidos para bin pelo tipo uint16_t.
    op = ULA_operacao;
    A = a;
    B = b;
    // shamt pode ser usado futuramente para operações de shift
    
    calculate();
}