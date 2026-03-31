#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>

class FileManager {
public:
    static bool createMasterKeyFile(const std::string& masterKey);
    static bool fileExists(const std::string& filePath);
    static std::string readMasterKeyFile(const std::string& filePath);
};

#endif // FILE_MANAGER_H