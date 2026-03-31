#ifndef MASTER_KEY_MANAGER_H
#define MASTER_KEY_MANAGER_H

#include <string>

class MasterKeyManager {
public:
    static int firstRun();
    static bool checkMasterKey();
private:
    static const std::string MASTER_KEY_FILE;
};

#endif // MASTER_KEY_MANAGER_H