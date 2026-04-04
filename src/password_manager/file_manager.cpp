#include "password_manager/file_manager.h"
#include <fstream>
#include <filesystem>

bool FileManager::createMasterKeyFile(const std::string& masterKey) {
    std::string dir_path = "../src/data";
    if (!std::filesystem::exists(dir_path)) {
        std::filesystem::create_directories(dir_path);
    }

    std::string file_path = dir_path + "/master_key.txt";
    std::ofstream outFile(file_path, std::ios::out);
    if (outFile.is_open()) {
        outFile << masterKey;
        outFile.close();
        return true;
    }
    return false;
}

bool FileManager::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

std::string FileManager::readMasterKeyFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    std::string storedKey;
    if (inFile.is_open()) {
        std::getline(inFile, storedKey);
        inFile.close();
    }
    return storedKey;
}