# ==============================================================================
# Makefile - Simulador de Sistema Operacional & CPU RISC 16-bits
# ==============================================================================

# Compilador e Flags
CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -Wpedantic -O2 -I. -Isrc
LDFLAGS   := 

# Diretórios
SRC_DIR   := src
CPU_DIR   := $(SRC_DIR)/cpu
REG_DIR   := $(CPU_DIR)/registers
ULA_DIR   := $(CPU_DIR)/ula
CONTROL_DIR := $(CPU_DIR)/control
TESTS_DIR := tests
BUILD_DIR := build

# Executável principal
TARGET    := simulador

# Fontes dos módulos (excluindo main.cpp)
COMMON_SRCS := \
    $(SRC_DIR)/so.cpp \
    $(SRC_DIR)/memory.cpp \
    $(CONTROL_DIR)/control_unit.cpp \
    $(CPU_DIR)/cpu.cpp \
    $(REG_DIR)/register_bank.cpp \
    $(REG_DIR)/register_table.cpp \
    $(ULA_DIR)/ula.cpp

# Objetos correspondentes em build/
COMMON_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(COMMON_SRCS))
MAIN_OBJ    := $(BUILD_DIR)/main.o

# Objetos específicos para testes
TEST_ULA_BIN     := $(BUILD_DIR)/test_ula
TEST_REG_BIN     := $(BUILD_DIR)/test_registers
TEST_CPU_BIN     := $(BUILD_DIR)/test_cpu
TEST_CONTROL_BIN := $(BUILD_DIR)/test_control_unit

.PHONY: all run test test_ula test_registers test_cpu test_control_unit clean help

# Target padrão
all: $(TARGET)

# ------------------------------------------------------------------------------
# Regra de Ligação do Executável Principal
# ------------------------------------------------------------------------------
$(TARGET): $(MAIN_OBJ) $(COMMON_OBJS)
	@echo "==> Ligando executável: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "==> Compilação concluída com sucesso: ./$(TARGET)"

# ------------------------------------------------------------------------------
# Regra Genérica para Objetos C++ (.cpp -> .o)
# ------------------------------------------------------------------------------
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "==> Compilando: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------------------------------------------------------------
# Execução do Simulador
# ------------------------------------------------------------------------------
run: $(TARGET)
	@echo "==> Executando $(TARGET)..."
	@./$(TARGET)

# ------------------------------------------------------------------------------
# Testes Automatizados
# ------------------------------------------------------------------------------
$(TEST_ULA_BIN): $(BUILD_DIR)/$(TESTS_DIR)/test_ula.o $(BUILD_DIR)/$(ULA_DIR)/ula.o
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_REG_BIN): $(BUILD_DIR)/$(TESTS_DIR)/test_registers.o $(BUILD_DIR)/$(REG_DIR)/register_bank.o $(BUILD_DIR)/$(REG_DIR)/register_table.o
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_CPU_BIN): $(BUILD_DIR)/$(TESTS_DIR)/test_cpu_integration.o $(COMMON_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_CONTROL_BIN): $(BUILD_DIR)/$(TESTS_DIR)/test_control_unit.o \
                      $(BUILD_DIR)/$(CONTROL_DIR)/control_unit.o \
                      $(BUILD_DIR)/$(SRC_DIR)/memory.o \
                      $(BUILD_DIR)/$(REG_DIR)/register_bank.o \
                      $(BUILD_DIR)/$(ULA_DIR)/ula.o
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

test_ula: $(TEST_ULA_BIN)
	@echo ""
	@echo "--- Executando Testes da ULA ---"
	@./$(TEST_ULA_BIN)

test_registers: $(TEST_REG_BIN)
	@echo ""
	@echo "--- Executando Testes dos Registradores ---"
	@./$(TEST_REG_BIN)

test_cpu: $(TEST_CPU_BIN)
	@echo ""
	@echo "--- Executando Teste de Integração da CPU ---"
	@./$(TEST_CPU_BIN)

test_control_unit: $(TEST_CONTROL_BIN)
	@echo ""
	@echo "--- Executando Testes da Unidade de Controle ---"
	@./$(TEST_CONTROL_BIN)

# Executa todos os testes
test: test_registers test_ula test_control_unit test_cpu
	@echo ""
	@echo "=========================================="
	@echo "  TODOS OS TESTES PASSARAM COM SUCESSO!   "
	@echo "=========================================="

# ------------------------------------------------------------------------------
# Limpeza
# ------------------------------------------------------------------------------
clean:
	@echo "==> Limpando arquivos de build..."
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "==> Limpeza concluída."

# ------------------------------------------------------------------------------
# Ajuda
# ------------------------------------------------------------------------------
help:
	@echo "Comandos disponíveis no Makefile:"
	@echo "  make                - Compila o simulador principal"
	@echo "  make run            - Compila e executa o simulador"
	@echo "  make test           - Compila e executa todas as suites de teste"
	@echo "  make test_ula       - Executa apenas os testes unitários da ULA"
	@echo "  make test_registers - Executa apenas os testes dos Registradores"
	@echo "  make test_control_unit - Executa apenas os testes da Unidade de Controle"
	@echo "  make test_cpu       - Executa o teste de integração da CPU"
	@echo "  make clean          - Remove os binários e diretório de build"
	@echo "  make help           - Exibe esta mensagem de ajuda"