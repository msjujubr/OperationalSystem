#ifndef REGISTER_TABLE_HPP
#define REGISTER_TABLE_HPP

#include <string>
#include <unordered_map>
#include <cstdint>

/**
 * @brief Tabela de Símbolos para Mapeamento Bidirecional de Registradores.
 */
class RegisterTable {
public:
    enum SpecialRegs : uint8_t {
        SPECIAL_REG_PC = 100,
        SPECIAL_REG_IR = 101
    };

private:
    std::unordered_map<std::string, uint8_t> nameToIndex;
    std::unordered_map<uint8_t, std::string> indexToName;

    static std::string sanitizeAndUpper(const std::string& str);

public:
    RegisterTable();

    /**
     * @brief Obtém o índice do registrador a partir do nome textual.
     * @param regName Nome (ex: "R0", "r0", "$r0", "PC", "IR").
     * @return Código numérico correspondente.
     * @throws std::invalid_argument se o nome for desconhecido.
     */
    uint8_t getIndex(const std::string& regName) const;

    /**
     * @brief Obtém o nome canônico do registrador a partir de seu código.
     * @param index Código do registrador (0..7, 100, 101).
     * @return Nome formatado (ex: "R0", "PC").
     * @throws std::out_of_range se o índice for inválido.
     */
    std::string getName(uint8_t index) const;

    /** Verifica se o nome informado é um registrador válido */
    bool isValidName(const std::string& regName) const;

    /** Verifica se o código numérico informado é válido */
    bool isValidIndex(uint8_t index) const;

    /** Verifica se o índice corresponde a um registrador de uso geral (0..7) */
    bool isGeneralPurpose(uint8_t index) const;
};

#endif // REGISTER_TABLE_HPP
