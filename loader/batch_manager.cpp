#include "batch_manager.hpp"

#include "clock_types.hpp"
#include "clock_core.hpp"
#include "metrics.hpp"
#include "parser.hpp"
#include "ram_loader.hpp"
#include "reporter.hpp"

#include "../src/cpu.hpp"
#include "../src/defines.hpp"
#include "../src/memory.hpp"

#include <cstddef>
#include <exception>
#include <iostream>

/**
 * BatchManager - Gerenciador do ciclo de vida em lote
 *
 * Percorre a fila de jobs executando cada um de forma isolada. A cada job o
 * contexto da máquina é reiniciado (PC = 512, registradores zerados, clock do
 * job zerado), de modo que nenhum resíduo do job anterior influencie o próximo.
 *
 * Princípio do processamento em lote: a falha de um job não derruba o lote.
 * Erros de leitura, de carga na RAM ou interrupções de hardware abortam apenas
 * o job corrente, e a execução prossegue para o próximo da fila.
 */

namespace {

/** Arquivo de saída persistente exigido pela especificação */
const char* ARQUIVO_SAIDA = "output.dat";

/**
 * Limite de instruções por job (proteção do lote).
 * Um job cujo fluxo nunca alcance o HALT (ex.: laço JUMP mal formado) travaria
 * o simulador indefinidamente. Ao atingir o teto o job é abortado e o lote segue.
 */
const uint32_t LIMITE_INSTRUCOES_POR_JOB = 1000000;

} // namespace

void BatchManager::executarLote(const std::vector<std::string>& listaArquivosJobs) {
    CPU cpu;

    std::cout << "===================================================" << std::endl;
    std::cout << "[SIMULADOR RISC 16-BITS] - EXECUCAO EM LOTE" << std::endl;
    std::cout << "===================================================" << std::endl;

    for (std::size_t i = 0; i < listaArquivosJobs.size(); ++i) {
        const std::string& caminho = listaArquivosJobs[i];
        const int idJob = static_cast<int>(i) + 1;

        // --- 1. Leitura do job em disco (JobParser) ------------------------
        JobData job;
        if (!JobParser::carregarArquivo(caminho, job)) {
            std::cout << "[LOTE] Falha ao ler o job " << idJob << ": " << caminho
                      << " (job ignorado)" << std::endl;
            continue;
        }

        job.id = idJob;
        if (job.nomeArquivo.empty()) {
            job.nomeArquivo = caminho;
        }

        std::cout << "Carregando Job " << job.id << ": " << job.nomeArquivo << "..." << std::endl;

        // --- 2. Reset de contexto -----------------------------------------
        // Zera o clock do job, abre a telemetria e devolve a CPU ao estado
        // inicial. O PC é posicionado explicitamente na primeira instrução do
        // job (endereço base 512, logo após a área reservada do SO).
        ClockCore::reset();
        MetricsTracker::iniciarJob(job.id, job.nomeArquivo);
        cpu.reset();
        cpu.setPC(OS_RESERVED_MEM);

        // --- 3. Carga das instruções e variáveis na RAM (RAMLoader) --------
        const bool cargaConcluida = RAMLoader::carregarNaRAM(job, RAM, TAM_RAM);
        if (!cargaConcluida) {
            std::cout << "[LOTE] Falha ao carregar o job " << job.id
                      << " na RAM (job abortado)" << std::endl;
            MetricsTracker::marcarErroKernel();
        }

        // --- 4. Execução do job -------------------------------------------
        if (cargaConcluida) {
            uint32_t instrucoesDoJob = 0;
            try {
                while (!cpu.isHalted()) {
                    if (instrucoesDoJob >= LIMITE_INSTRUCOES_POR_JOB) {
                        std::cout << "[LOTE] Job " << job.id
                                  << " excedeu o limite de instrucoes sem HALT (job abortado)"
                                  << std::endl;
                        MetricsTracker::marcarErroKernel();
                        break;
                    }

                    cpu.step();

                    // A instrução só é contabilizada após concluir sem interrupção.
                    MetricsTracker::registrarInstrucao();
                    ++instrucoesDoJob;
                }

                if (cpu.isHalted()) {
                    std::cout << "Execucao finalizada (HALT encontrado)." << std::endl;
                }
            } catch (const std::exception& e) {
                // Interrupção de hardware (SEGFAULT na área do SO ou opcode inválido).
                // Encerra somente este job e preserva as métricas coletadas até aqui.
                std::cout << "[INTERRUPCAO GERADA PELO HARDWARE]: " << e.what() << std::endl;
                MetricsTracker::marcarErroKernel();
            }
        }

        // --- 5. Relatório do job ------------------------------------------
        // Pendente de integração: MetricsTracker::capturarEstadoCPU(PC, IR, R)
        // não pode ser chamado aqui porque a CPU atual mantém PC, IR e R0..R7
        // privados e não expõe nenhum acessor. Será ligado quando o módulo de
        // CPU disponibilizar o RegisterBank (getContext / getRegistersSnapshot).
        const JobMetrics metricas = MetricsTracker::obterMetricas();
        Reporter::exibirTerminal(metricas);
        Reporter::salvarOutputDat(ARQUIVO_SAIDA, metricas);

        std::cout << "===================================================" << std::endl;
    }
}
