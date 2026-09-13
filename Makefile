# MAKEFILE - SIMULADOR DE SISTEMA OPERACIONAL
CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++11

TARGET = simulador

BUILD_DIR = build

SOURCES = main.cpp \
          src/cpu.cpp \
          src/memory.cpp \
          src/so.cpp

OBJECTS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/cpu.o \
          $(BUILD_DIR)/memory.o \
          $(BUILD_DIR)/so.o

# COMPILACAO PRINCIPAL

all: $(TARGET)

# GERA O EXECUTAVEL

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)


# COMPILACAO DOS OBJETOS

$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c main.cpp -o $(BUILD_DIR)/main.o


$(BUILD_DIR)/cpu.o: src/cpu.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c src/cpu.cpp -o $(BUILD_DIR)/cpu.o


$(BUILD_DIR)/memory.o: src/memory.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c src/memory.cpp -o $(BUILD_DIR)/memory.o


$(BUILD_DIR)/so.o: src/so.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c src/so.cpp -o $(BUILD_DIR)/so.o


# CRIA A PASTA BUILD

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# EXECUTA O SIMULADOR

run: $(TARGET)
	./$(TARGET)

# EXECUTA OS TESTES

test: $(TARGET)
	./$(TARGET)

# LIMPEZA

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

# RECOMPILACAO COMPLETA

rebuild: clean all


.PHONY: all run test clean rebuild