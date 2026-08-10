#include "src/so.hpp"
#include <iostream>
#include <vector>
#include <cstdint>

int main() {
    SistemaOperacional os;

    // JOB 1: Soma simples (A + B = C)
    // Endereços relativos ao Job: 10 (A), 11 (B), 12 (C)
    std::vector<uint16_t> job1_instrucoes = {
        0x100A, // LOAD R0, [10]   → R0 = A
        0x110B, // LOAD R1, [11]   → R1 = B
        0x2201, // ADD  R2, R0, R1 → R2 = A + B
        0x320C, // STORE R2, [12]  → C = R2
        0xF000  // HALT
    };
    std::vector<uint16_t> job1_dados = {5, 7};   // A = 5, B = 7

    os.executarJob(1, "log/soma_simples.txt", job1_instrucoes, job1_dados);

    // JOB 2: Demonstração da Trava de Segurança (SO)
    // Tenta acessar um endereço na área reservada do SO (endereço 150 < 512)
    std::cout << "Iniciando Job 2 (Simulando ataque ao Kernel)..." << std::endl;
    try {
        // O endereço 150 está na área protegida [0, 511]
        LerMemoria(150);
    } catch (const std::exception& e) {
        std::cout << "[INTERRUPCAO GERADA PELO HARDWARE]: " << e.what() << std::endl;
    }

    return 0;
}