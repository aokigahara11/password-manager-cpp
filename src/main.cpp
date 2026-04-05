#include "password_manager/password_manager.h"
#include "password_manager/master_key_manager.h"
#include "password_manager/file_manager.h"
#include "ui/ui.h"
#include <iostream>

int main() {
    const std::string master_key_file = "../src/data/master_key.txt";

    // Проверка существования мастер-ключа
    if (!FileManager::fileExists(master_key_file)) {
        // Первый запуск - создание мастер-ключа
        if (MasterKeyManager::firstRun() != 0) {
            std::cerr << "Failed to set up master key. Exiting." << std::endl;
            return 1;
        }
    } else {
        // Проверка мастер-ключа при существующем файле
        if (!MasterKeyManager::checkMasterKey()) {
            std::cerr << "Master key verification failed. Exiting." << std::endl;
            return 1;
        }
    }

    // Инициализация менеджера паролей с путём к БД
    PasswordManager pm("../src/data/passwords.db");
    
    // Запуск главного меню UI
    UI::MainMenu(pm);
    
    return 0;
}