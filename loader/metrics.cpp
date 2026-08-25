#include "metrics.hpp"
#include "clock_core.hpp"

JobMetrics MetricsTracker::metricasAtuais = {};

void MetricsTracker::iniciarJob(int id, const std::string& nome) {
    metricasAtuais = JobMetrics{};
    metricasAtuais.idJob = id;
    metricasAtuais.nomeJob = nome;
}

void MetricsTracker::registrarInstrucao() {
    metricasAtuais.instrucoesExecutadas++;
}
void MetricsTracker::registrarAcessoRAM() {
    metricasAtuais.acessosRAM++;
}
void MetricsTracker::registrarAcessoDisco() {
    metricasAtuais.acessosDisco++;
}
void MetricsTracker::capturarEstadoCPU(uint16_t pc, uint16_t ir, const uint16_t regs[8]) {
    metricasAtuais.pcFinal = pc;
    metricasAtuais.irFinal = ir;
    for (int i = 0; i < 8; ++i) {
        metricasAtuais.registradores[i] = regs[i];
    }
}
void MetricsTracker::marcarErroKernel() {
    metricasAtuais.erroKernel = true;
}
JobMetrics MetricsTracker::obterMetricas() {
    metricasAtuais.clockTotal = ClockCore::getCycles();
    return metricasAtuais;
}