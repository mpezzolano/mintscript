#include <fstream>

#include "module.h"
#include "error.h"
#include "parser.h"
#include "ast.h"
#include "debug_visitor.h"
#include "globals.h"

mType* mModule::Type = new mType(
    "Module",
    []() {
    },
    []() -> mObject* {
        return new mModule();
    }
);

mModule::mModule() : mObject(mModule::Type) { }

std::string ReadFile(const std::filesystem::path& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        mError::AddError("Failed to open file: " + path.string());
        return "";
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return source;
}

mObject *mModule::ImportFile(const std::filesystem::path& path) {
    std::filesystem::path filePath(MAIN_FILE_PATH / path);

    // Check if the file was already imported
    if (mSymbolTable::GetScope(filePath.string()) != nullptr) {
        // TODO: Return the module
        return nullptr;
    }

    // Read the file
    std::string source = ReadFile(filePath);
    
    // Parse the file
    Parser parser(source);
    ASTNode *node = parser.Parse();

    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        return nullptr;
    }
    
    if (node == nullptr) {
        mError::AddError("Failed to parse file: " + path.string());
        return nullptr;
    }
    
    // Evaluate the AST
    mModule* module = new mModule();
    mSymbolTable* symbolTable = mSymbolTable::NewScope(filePath.string());
    EvalVisitor::Eval(node, symbolTable, module);
    
    // Check for errors
    if (mError::HasError()) {
        mError::PrintErrors();
        return nullptr;
    }

    return module;
}

mObject *mModule::Import(const std::string &module_name) {
    // Get the module path
    std::filesystem::path modulePath = mModule::GetModulePath(module_name);

    if (modulePath == "") {
        mError::AddError("Failed to find module: " + module_name);
        return nullptr;
    }

    // Check if the module was already imported
    // TODO: Check if the module was already imported

    // Read the module
    const std::filesystem::path rootPath = modulePath / "root.mint";

    mObject* module = mModule::ImportFile(rootPath.string());
    
    return module;
}

std::filesystem::path mModule::GetModulePath(const std::string &module_name) {
    std::filesystem::path modulePath;

    modulePath = MINT_MODULE_PATH / module_name;

    if (std::filesystem::exists(modulePath) && std::filesystem::is_directory(modulePath)) {
        return modulePath;
    }

    modulePath = MAIN_FILE_PATH / module_name;

    if (std::filesystem::exists(modulePath) && std::filesystem::is_directory(modulePath)) {
        return modulePath;
    }

    return "";
}
