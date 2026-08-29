# Modulo Clock e Loader (Simulador RISC 16-bits)

Este diretorio contera os componentes do modulo **Clock e Loader** do simulador RISC 16-bits em lote.

## Estrutura de Arquivos Planejada

```text
loader/
├── clock_types.hpp        # Tipos e estruturas de dados de metricas e jobs
├── clock_core.hpp / .cpp  # Motor do relogio global e penalidades de ciclo
├── metrics.hpp / .cpp     # Telemetria e rastreamento de metricas por Job
├── reporter.hpp / .cpp    # Formatacao de saida no terminal e gravacao do output.dat
├── parser.hpp / .cpp      # Leitura de arquivos de jobs (.txt) e simulacao de disco
├── ram_loader.hpp / .cpp  # Alocacao na RAM fisica (a partir do endereco 512)
├── batch_manager.hpp / .cpp # Gerenciamento da fila de lote e reset de contexto
└── tests/                 # Cargas de teste (.txt) e testes unitarios do modulo
```

## Diretrizes de Desenvolvimento
- Branch oficial do modulo: `clock-loader`
- Cada integrante implementa seu respectivo arquivo `.cpp` evitando conflitos de merge.
- Apos validacao local com as cargas de teste, sera aberto Pull Request para a branch `main`.
