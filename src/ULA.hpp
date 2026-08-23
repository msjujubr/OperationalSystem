#ifndef ULA_HPP
#define ULA_HPP
#include <cstdint> // essa biblioteca serve para declarar variáveis de tamanho X bits, EX: uint32_t a => cria a variavel "a", que será sempre de 32 bits

enum operation //as operações que serão executadas pela ula
{
    // operações básicas
    ADD, // soma A + B
    SUB, // subtrai A - B
    MUL, // multiplica A x B
    DIV,// divide A / B
    AND_OP, // faz A AND B
    OR_OP,
    // operações de fluxo do programa. Elas nao produzem resultado numérico e B é um registrador, isso implica que qualquer valor que tiver lá vai ser usado
    BEQ, // Branch if equal => desvia se A == B
    BNE,  // Branch if Not equal => desvia se A != B
    BLT, // Branch if Less Than => desvia se A < B
    BGT, // Branch if Greater Than => desvia se A > B 
    JUMP, //pula, literalmente

    // comparações de imediato, B é uma constante que está na instrução
    BGTI, // BGTI immediate - Compara A > B (B é usado como imediato)
    BLTI, // BLTI immediate - Compara A < B (B é usado como imediato)

    // instruções de memória
    LW,   // Load word - calcula endereço (base + offset), basicamente pega um valor da memória e coloca no registrador
    LA,   // Load adress - similar a LW (retorna enderedeço efetivo), ou seja, pega o endereço da memoria e coloca no registrador. NÃO CONFUNDIR COM LW
    ST,    // Store - calcula endereço para gravação (base + offset), pega um valor do registrador e coloca na memória
    HALT
};

class ULA
{
public:
    // -> Entradas (observa-se que são aceitos apenas numeros positivos, indicados por uint16_t)
    uint16_t A = 0;
    uint16_t B = 0;
    operation op = ADD;

    // -> Saídas (observa-se que podem existir resultados negativos, indicados por int16_t)
    int16_t result = 0;    // Resultado (interpretação: signed 16-bit na maioria dos casos)
    bool overflow = false; // Indica overflow aritmético (ou erro, como divisão por zero)

    // -> Métodos principais
    void calculate();
    void execute(operation ULA_operacao, uint16_t a, uint16_t b, uint16_t shamt = 0); // A e B devem ser decimais.
    // shamt = Shift AMounT = Quantidade de Deslocamento de bits, para a esquerda
};
#endif // ULA_HPP


// Fluxograma de como funciona a ULA

// ┌─────────────────────────────────────────────────────────────┐
// │                     VOCÊ ESCREVE:                           │
// │                                                             │
// │   ula.execute(ADD, 10, 5, 0);                              │
// │                      ↑   ↑                                  │
// │                  decimal decimal                            │
// └─────────────────────────────────────────────────────────────┘
//                               │
//                               ▼
// ┌─────────────────────────────────────────────────────────────┐
// │                   COMPILADOR CONVERTE:                      │
// │                                                             │
// │   Como a função espera uint16_t, o compilador faz:         │
// │                                                             │
// │   10 → 0b0000000000001010 (0x000A)                        │
// │   5  → 0b0000000000000101 (0x0005)                        │
// │   0  → 0b0000000000000000 (0x0000)                        │
// │                                                             │
// │   Chama:                                                    │
// │   execute(ADD, 0x000A, 0x0005, 0x0000)                    │
// └─────────────────────────────────────────────────────────────┘
//                               │
//                               ▼
// ┌─────────────────────────────────────────────────────────────┐
// │                    ULA RECEBE:                              │
// │                                                             │
// │   void ULA::execute(operation ALUop,                       │
// │                     uint16_t a,    ← 0x000A (binário)      │
// │                     uint16_t b,    ← 0x0005 (binário)      │
// │                     uint16_t shamt)← 0x0000 (binário)      │
// │   {                                                        │
// │       A = a;  // A = 0b0000000000001010                   │
// │       B = b;  // B = 0b0000000000000101                   │
// │       calculate();                                         │
// │   }                                                        │
// └─────────────────────────────────────────────────────────────┘
//                               │
//                               ▼
// ┌─────────────────────────────────────────────────────────────┐
// │                    ULA CALCULA:                             │
// │                                                             │
// │   case ADD:                                                 │
// │       // A e B já estão em binário!                        │
// │       int32_t res32 = (int32_t)A + (int32_t)B;            │
// │       // 0b...1010 + 0b...0101 = 0b...1111                │
// │       // = 15 em decimal                                   │
// │       result = (uint16_t)res32;  // 0b0000000000001111    │
// └─────────────────────────────────────────────────────────────┘
//                               │
//                               ▼
// ┌─────────────────────────────────────────────────────────────┐
// │                    VOCÊ VÊ O RESULTADO:                    │
// │                                                             │
// │   cout << result;  // 15 (decimal)                │
// │                                                             │
// │   O computador converte de volta para decimal para         │
// │   você entender!                                           │
// └─────────────────────────────────────────────────────────────┘