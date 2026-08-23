#include "register_table.hpp"
#include "register_bank.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>

std::string RegisterTable::sanitizeAndUpper(const std::string& str) {
    if (str.empty()) return "";

    // Trim de espaços no início e fim, bem como vírgulas residuais
    size_t first = str.find_first_not_of(" \t\r\n,");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n,");
    std::string trimmed = str.substr(first, (last - first + 1));

    // Conversão segura para maiúsculas (evitando UB com cast para unsigned char)
    std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });

    return trimmed;
}

RegisterTable::RegisterTable() {
    // Registradores Gerais (R0..R7)
    for (uint8_t i = 0; i < RegisterBank::NUM_REGISTERS; ++i) {
        std::string canonical = "R" + std::to_string(i);
        
        nameToIndex[canonical] = i;
        indexToName[i] = canonical;

        // Variações suportadas com prefixo explícito: "$R0", "$0"
        nameToIndex["$" + canonical] = i;
        nameToIndex["$" + std::to_string(i)] = i;
    }

    // Registradores Especiais
    nameToIndex["PC"] = SPECIAL_REG_PC;
    nameToIndex["$PC"] = SPECIAL_REG_PC;
    indexToName[SPECIAL_REG_PC] = "PC";

    nameToIndex["IR"] = SPECIAL_REG_IR;
    nameToIndex["$IR"] = SPECIAL_REG_IR;
    indexToName[SPECIAL_REG_IR] = "IR";
}

bool RegisterTable::isValidName(const std::string& regName) const {
    std::string normalized = sanitizeAndUpper(regName);
    if (normalized.empty()) return false;
    return nameToIndex.find(normalized) != nameToIndex.end();
}

bool RegisterTable::isValidIndex(uint8_t index) const {
    return indexToName.find(index) != indexToName.end();
}

bool RegisterTable::isGeneralPurpose(uint8_t index) const {
    return index < RegisterBank::NUM_REGISTERS;
}

uint8_t RegisterTable::getIndex(const std::string& regName) const {
    std::string normalized = sanitizeAndUpper(regName);
    auto it = nameToIndex.find(normalized);
    if (it != nameToIndex.end()) {
        return it->second;
    }
    throw std::invalid_argument("RegisterTable::getIndex - Registrador desconhecido: '" + regName + "'");
}

std::string RegisterTable::getName(uint8_t index) const {
    auto it = indexToName.find(index);
    if (it != indexToName.end()) {
        return it->second;
    }
    throw std::out_of_range("RegisterTable::getName - Indice invalido: " + std::to_string(index));
}
