#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "defines.hpp"
#include <vector>
#include <cstdint>

// Declaração global da RAM
extern uint16_t RAM[TAM_RAM];

// Estatísticas globais do sistema
extern int clockGlobal;
extern int instExecutadas;
extern int acessosMemoria;
extern int acessosDisco;

//Interface mediadora de leitura da RAM.
//Aplica a penalidade de ciclo de instrução (clock).

uint16_t LerMemoria(uint16_t endereco);

//Interface mediadora de escrita da RAM.
//Aplica a penalidade de ciclo de instrução (clock).
void EscreverMemoria(uint16_t endereco, uint16_t dado);

//Interface de Disco Simulado.
//Simula a busca mecânica, aplicando alta penalidade de I/O ao clock.
int LerDisco(uint16_t dadoBuscado, const std::vector<uint16_t>& Disco);

#endif // MEMORY_HPP