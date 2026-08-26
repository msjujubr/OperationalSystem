#include "ram_loader.hpp"

// Endereço base da RAM física após a área reservada ao Sistema Operacional.
// (Mesmo valor citado no comentário do ram_loader.hpp: OS_RESERVED_MEM = 512)
static const uint16_t OS_RESERVED_MEM = 512;

bool RAMLoader::carregarNaRAM(const JobData& job, uint16_t* ram, uint16_t tamRam) {
    if (ram == nullptr) {
        return false;
    }

    // 1) Copiar as instruções para a RAM, a partir do endereço base (512)
    uint32_t enderecoInstrucao = OS_RESERVED_MEM;

    for (uint16_t instrucao : job.instrucoes) {
        if (enderecoInstrucao >= tamRam) {
            // Job não cabe na RAM disponível
            return false;
        }
        ram[enderecoInstrucao] = instrucao;
        enderecoInstrucao++;
    }

    // 2) Posicionar as variáveis nos endereços corretos.
    // O endereço de cada variável é relativo à base (512), conforme
    // documentado no campo `variaveis` de JobData.
    for (const auto& variavel : job.variaveis) {
        uint16_t enderecoRelativo = variavel.first;
        uint16_t valor = variavel.second;

        uint32_t enderecoAbsoluto = static_cast<uint32_t>(OS_RESERVED_MEM) + enderecoRelativo;

        if (enderecoAbsoluto >= tamRam) {
            // Endereço da variável estoura o limite da RAM
            return false;
        }

        ram[enderecoAbsoluto] = valor;
    }

    return true;
}
