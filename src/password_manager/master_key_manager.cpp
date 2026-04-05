#include "password_manager/master_key_manager.h"
#include "password_manager/file_manager.h"
#include "utils/password_generator.h"
#include "utils/clipboard_manager.h"
#include "utils/input_reader.h"
#include "utils/validator.h"
#include <iostream>

const std::string MasterKeyManager::MASTER_KEY_FILE = "../src/data/master_key.txt";

int MasterKeyManager::firstRun() {
    std::cout << "Welcome to password-manager-cpp!" << std::endl;
    std::cout << "For management need create master key." << std::endl;
    std::cout << "Enter your master key or generate? (1/2): ";
    int choice;
    int length_master_key;
    
    if (!InputReader::readIntInRange(choice, 1, 2)) {
        while (!InputReader::readIntInRange(choice, 1, 2)) {
            std::cout << "Please enter a valid choice (1 or 2): ";
        }
    }

    if (choice == 1) {
        std::string master_key;
        std::cout << "Enter your master key: ";
        
        if (!InputReader::readLine(master_key)) {
            std::cout << "Error reading master key or invalid master key. Please try again." << std::endl;
            return 1;
        }

        // Проверка валидности мастер-ключа
        if (!Validator::IsVaildPassword(master_key)) {
            std::cout << "Want to generate a new master key? (y/n): ";
            char gen_choice;
            
            if (InputReader::readChar(gen_choice) && (gen_choice == 'y' || gen_choice == 'Y')) {
                std::cout << "Enter length of master key: ";
                if (InputReader::readInt(length_master_key)) {
                    while (!Validator::IsVaildPasswordLength(length_master_key)) {
                        std::cout << "Enter length of master key: ";
                        InputReader::readInt(length_master_key);
                    }
                }

                std::cout << "Your generated master key: " << master_key << std::endl;
            }
            return 1;
        }

        if (!FileManager::createMasterKeyFile(master_key)) {
            std::cout << "Error saving master key to file." << std::endl;
            return 1;
        }

        char copy_choice;
        std::cout << "Do you want to copy the master key to clipboard? (y/n): ";
        if (InputReader::readChar(copy_choice) && (copy_choice == 'y' || copy_choice == 'Y')) {
            if (ClipboardManager::copyToClipboard(master_key)) {
                std::cout << "Master key copied to clipboard." << std::endl;
            } else {
                std::cout << "Failed to copy master key to clipboard." << std::endl;
            }
        }

    } else if (choice == 2) {
        std::cout << "Enter length of master key: ";
        
        if (InputReader::readInt(length_master_key)) {
            while (!Validator::IsVaildPasswordLength(length_master_key)) {
                std::cout << "Enter length of master key: ";
                InputReader::readInt(length_master_key);
            }
        }

        std::string master_key = PasswordGenerator::generatePassword(length_master_key);
        std::cout << "Your generated master key: " << master_key << std::endl;
        if (!FileManager::createMasterKeyFile(master_key)) {
            std::cout << "Error saving master key to file." << std::endl;
            return 1;
        }
        if (ClipboardManager::copyToClipboard(master_key)) std::cout << "Master key copied to clipboard." << std::endl;
    }
    return 0;
}

bool MasterKeyManager::checkMasterKey() {
    int attempts = 0;

    while (attempts < 5) {
        std::string input_key;
        std::cout << "Enter master key: ";
        
        if (!InputReader::readLine(input_key)) {
            std::cout << "Error reading master key. Please try again." << std::endl;
            continue;
        }

        std::string stored_key = FileManager::readMasterKeyFile(MASTER_KEY_FILE);

        if (input_key == stored_key) {
            std::cout << "Access granted!" << std::endl;
            return true;
        } else {
            std::cout << "Incorrect master key. Try again." << std::endl;
            attempts++;
        }

        if (attempts == 5) {
            std::cout << "Too many failed attempts. Exiting program." << std::endl;
            return false;
        }
    }
    return false;
}