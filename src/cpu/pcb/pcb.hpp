#ifndef PCB_HPP
#define PCB_HPP

// PCB (PROCESS CONTROL BLOCK)
// Estrutura de dados responsável por armazenar informações para localizar e gerenciar processos.

#include <iostream>

enum class Process_State {
    Ready,
    Running,
    Stopped,
    Blocked
};

class PCB {
    private:
        // nome do processo
        std::string name;
        // contador do programa que indica qual instrução do processo deve ser executada
        int PC;
        // número único que identifica o processo
        int PID;
        // estado do processo: rodando, parado, esperando
        Process_State state = Process_State::Ready;
        // o nível de prioridade de um processo mede quanta atenção e tempo de processamente um processo recebe
        int priority = 0;
        // por quanto tempo o processo deve ser executado até outro processo ser executado
        int quantum = 0;
};

#endif