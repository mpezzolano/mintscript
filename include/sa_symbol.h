#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <list>

namespace sa {
    class SymbolTable;
    class Type;

    class Field {
    public:
        bool isMutable;
        std::string name;
        int offset;
        Type* type;

        Field() = default;
        Field(bool isMutable, Type* type);
    };

    class Method {
    public:
        std::string name;
        Type* type;
        SymbolTable* parameters = nullptr;

        Method() = default;
        Method(Type* type) { this->type = type; }
        Method(std::string name, Type* type) { this->name = name; this->type = type; }
    };

    class Type {
    public:
        std::string name;
        std::unordered_map<std::string, Method> methods;
        std::unordered_map<std::string, Field> fields;
        std::vector<Type*> variants;
        std::vector<Type*> typeParameters;
        Type* parent = nullptr;

        Type();
        Type(const std::string& name);
        ~Type() = default;

        void SetMethod(std::string name, sa::Method symbol);
        
        sa::Method* GetMethod(std::string name);

        bool HasMethod(std::string name);

        void AddField(std::string name, sa::Field symbol);

        void SetField(std::string name, sa::Field symbol);
        
        sa::Field* GetField(std::string name);

        bool HasField(std::string name);

        bool IsVariantOf(Type* type);

        Type* GetVariant(std::vector<sa::Type*> types);

        std::string ToString();

    private:
        int lastFieldOffset = 0;
    };

    class Symbol {
    public:
        bool isMutable;
        std::string name;
        Type* type;

        Symbol() = default;
    };

    class Module {
    public:
        std::string name;
        SymbolTable* symbols;
    };

    class SymbolTable {
    public:
        SymbolTable() = default;
        SymbolTable(SymbolTable* parent) { this->parent = parent; }

        inline void SetSymbol(std::string name, Symbol symbol) { symbols[name] = symbol; }

        Symbol* GetSymbol(std::string name);

        inline void SetParent(SymbolTable* parent) { this->parent = parent; }

        inline SymbolTable* GetParent() { return parent; }

        inline void SetType(std::string name, Type symbol) { types[name] = symbol; }

        Type* GetType(std::string name);

        Type* GetTypeVariant(std::string name, std::vector<sa::Type*> types);

        void SetModule(std::string name, Module symbol);

        Module* GetModule(std::string name);

    private:
        SymbolTable* parent = nullptr;
        std::unordered_map<std::string, Type> types;
        std::unordered_map<std::string, Symbol> symbols;
        std::unordered_map<std::string, Module> modules;
    };

    extern SymbolTable* global;
}