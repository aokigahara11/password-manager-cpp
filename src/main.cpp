#include "password_manager/password_manager.h"
#include "utils/clipboard_manager.h"
#include "utils/password_generator.h"
#include "ui/ui.h"
#include "utils/validator.h"
#include "utils/input_reader.h"
#include <iostream>
#include <cstring>

int main() {
    const std::string db_path = "../src/data/passwords.db";
    PasswordManager pm(db_path);

    int attempts = 0;
    const int max_attempts = 5;
    bool authenticated = false;
    bool first_run = !pm.databaseExists();
    int length_password = 0;

    while (attempts < max_attempts && !authenticated) {
        std::string master_key;
        
        if (first_run) {
            std::cout << "Database not found. Create a master key (Enter the length for password generation): ";
            while (!InputReader::readInt(length_password) || !Validator::IsVaildPasswordLength(length_password)) {
                std::cout << "Enter password length (8-128): ";
            }
            master_key = PasswordGenerator::generatePassword(length_password);
            std::cout << std::endl << "Master key generated: " << master_key << std::endl;
            std::cout << "Want you save master key to clipboard? (y/n): ";
            char save_clipboard;
            while (true) {
                if (!InputReader::readChar(save_clipboard)) {
                    std::cout << "Error reading input. Please try again: ";
                    continue;
                }
                if (save_clipboard == 'y' || save_clipboard == 'Y') {
                    if (ClipboardManager::copyToClipboard(master_key)) {
                        std::cout << "Master key copied to clipboard successfully!" << std::endl;
                        std::cout << "Please remember this master key. It will be required to access your password manager." << std::endl;
                    } else {
                        std::cout << "Failed to copy master key to clipboard." << std::endl;
                    }
                    break;
                } else if (save_clipboard == 'n' || save_clipboard == 'N') {
                    std::cout << "Please remember this master key. It will be required to access your password manager." << std::endl;
                    break;
                } else {
                    std::cout << "Invalid input. Please enter 'y' or 'n': ";
                }
            }
        } else {
            std::cout << std::endl << "Enter master key: ";
            while (!InputReader::readLine(master_key) || master_key.empty()) {
                std::cout << "Enter master key: ";
            }
        }
        
        if (pm.authenticate(master_key)) {
            authenticated = true;
            std::cout << "Access granted!" << std::endl;
        } else {
            std::cout << "Incorrect master key. Try again." << std::endl;
            attempts++;
        }

        // Затирание мастер-ключа в памяти
        memset(master_key.data(), 0, master_key.size());
        master_key.clear();
    }

    if (!authenticated) {
        std::cerr << "Too many failed attempts. Exiting program." << std::endl;
        return 1;
    }
    
    UI::MainMenu(pm);
    
    return 0;
}