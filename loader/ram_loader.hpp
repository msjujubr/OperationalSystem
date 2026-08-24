#ifndef RAM_LOADER_HPP
#define RAM_LOADER_HPP

#include "clock_types.hpp"
#include <cstdint>

/**
 * RAMLoader - Mapeamento e alocação de instruções e dados na RAM física (a partir de 512)
 * Integrante responsável: Integrante 6
 */
class RAMLoader {
public:
    // Carrega o JobData na RAM física a partir do endereço base (OS_RESERVED_MEM = 512)
    static bool carregarNaRAM(const JobData& job, uint16_t* ram, uint16_t tamRam);
};

#endif // RAM_LOADER_HPP
