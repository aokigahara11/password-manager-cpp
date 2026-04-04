#include "password_manager/master_key_manager.h"
#include "password_manager/file_manager.h"
#include "utils/password_generator.h"
#include "utils/clipboard_manager.h"
#include <iostream>

const std::string MasterKeyManager::MASTER_KEY_FILE = "../src/data/master_key.txt";

int MasterKeyManager::firstRun() {
    std::cout << "Welcome to password-manager-cpp!" << std::endl;
    std::cout << "For management need create master key." << std::endl;
    std::cout << "Enter your master key or generate? (1/2): ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        std::string master_key;
        std::cout << "Enter your master key: ";
        std::cin >> master_key;
        if (!FileManager::createMasterKeyFile(master_key)) {
            std::cout << "Error saving master key to file." << std::endl;
            return 1;
        }
        if (ClipboardManager::copyToClipboard(master_key)) std::cout << "Master key copied to clipboard." << std::endl;
    } else if (choice == 2) {
        int length_master_key;
        std::cout << "Enter length of master key: ";
        std::cin >> length_master_key;
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
        std::cin >> input_key;

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