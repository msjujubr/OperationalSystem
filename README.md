# Simulador RISC 16-bits com Processamento Batch
![C++](https://img.shields.io/badge/Linguagem-C++-green.svg)
![Make](https://img.shields.io/badge/Compilacao-Make-orange)
  


--- 

Simulador de uma máquina **RISC de 16 bits**, desenvolvido em **C++17**
para a disciplina de Sistemas Operacionais.

O projeto implementa uma CPU modular, memória RAM, processamento em lote
(*batch*), loader, clock, métricas e execução de programas em Assembly.


-   Arquitetura RISC de 16 bits
-   8 registradores gerais (`R0`--`R7`)
-   Registradores `PC` e `IR`
-   Memória de 65.536 palavras de 16 bits
    -   Área protegida do sistema: `0x0000`--`0x01FF`
    -   Área de usuário a partir de `0x0200`
-   Execução sequencial de Jobs
-   Simulação de clock e acessos à RAM/disco
-   Geração de métricas em `output.dat`

### Documentação

- [Documentação Final](docs/SO_Documentação.pdf)
- [Manual da Máquina — Assembly](docs/SO_ManualAssembly.pdf)


##  Estrutura

``` text
.
├── docs/           # Documentação
├── loader/         # Batch, clock, parser, métricas e relatório
├── src/
│   ├── cpu/        # CPU, ULA, controle e registradores
│   ├── memory.*    # Memória principal
│   └── so.*        # Sistema
├── tests/          # Testes
├── main.cpp
├── Makefile
├── output.dat
└── simulador
```


## Instruções

  Instrução     Opcode Descrição
  ----------- -------- ---------------------
  `LOAD`         `0x1` Carrega da memória
  `ADD`          `0x2` Soma
  `STORE`        `0x3` Armazena na memória
  `SUB`          `0x4` Subtração
  `AND`          `0x5` AND bit a bit
  `OR`           `0x6` OR bit a bit
  `BEQ`          `0x7` Desvio condicional
  `JUMP`         `0x8` Salto incondicional
  `HALT`         `0xF` Finaliza o Job


## Execução

O projeto utiliza **Make**.

### Compilar

``` bash
make
```

### Executar

``` bash
make run
```

### Executar os testes

``` bash
make test
```

Testes individuais:

``` bash
make test_ula
make test_registers
make test_control_unit
make test_cpu
make test_memoria
make test_cpu_memoria
```

### Limpar arquivos gerados

``` bash
make clean
```

Para consultar todos os comandos disponíveis:

``` bash
make help
```

## Jobs

Os programas são representados por arquivos `.txt` contendo instruções
hexadecimais de 16 bits.

Exemplo:

``` text
100A
110B
2201
320C
F000

.data
0A 0005
0B 0007
0C 0000
```

Os endereços dos Jobs são relativos à base `0x0200`.

Consulte o [Manual da Máquina --- Assembly](docs/SO_ManualAssembly.pdf)
para o formato completo dos programas e a codificação das instruções.

## Colaboradores

Os colaboradores são obtidos diretamente do histórico de contribuições
do repositório:

 ![Contributors](https://readme-contribs.as93.net/contributors/msjujubr/OperationalSystem)


------------------------------------------------------------------------
