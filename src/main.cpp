#include "password_manager/password_manager.h"
#include "password_manager/master_key_manager.h"
#include "utils/password_generator.h"
#include "password_manager/file_manager.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int mainMenu(PasswordManager& pm);
int menuForFindRecord(PasswordManager& pm);

int menuForFindRecord(PasswordManager& pm) {
    int choice;
    std::cout << "1 - Update record\n2 - Delete record\n3 - Back to main menu\n";
    std::cin >> choice;

    auto [foundService, foundMail, foundPass, index] = pm.findRecord();
    if (index < 0) return 0;

    if (choice == 1) {
        std::string choice_for_update;
        std::cout << "Choice parameter for update (SERVICE/MAIL/PASS/ALL): " << std::endl;
        std::cin >> choice_for_update;
        if (choice_for_update == "SERVICE") {
            std::string new_service;
            std::cout << "Enter new service: ";
            std::cin >> new_service;
            pm.updateRecord(index, "SERVICE", new_service);
        } else if (choice_for_update == "MAIL") {
            std::string new_mail;
            std::cout << "Enter new mail: ";
            std::cin >> new_mail;
            pm.updateRecord(index, "MAIL", new_mail);
        } else if (choice_for_update == "PASS") {
            int length_password;
            std::cout << "Enter length of new password: ";
            std::cin >> length_password;
            std::string new_pass = PasswordGenerator::generatePassword(length_password);
            pm.updateRecord(index, "PASS", new_pass);
        } else if (choice_for_update == "ALL") {
            auto [new_service, new_mail, new_password] = pm.enterRecord();
            pm.updateRecord(index, "SERVICE", new_service);
            pm.updateRecord(index, "MAIL", new_mail);
            pm.updateRecord(index, "PASS", new_password);
            std::cout << "Record fully updated." << std::endl;
        } else {
            std::cout << "Invalid parameter choice for update." << std::endl;
        }
    } else if (choice == 2) {
        pm.deleteRecord(index);
    } else if (choice == 3) {
        mainMenu(pm);
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
    }

    return 0;
}

int mainMenu(PasswordManager& pm) {
    srand(static_cast<unsigned int>(time(0)));

    while (true) {
        int choice;
        std::cout << "\nMain menu\n1 - Add\n2 - Show\n3 - Find\n4 - Delete all\n5 - Exit\nEnter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            auto [service, mail, password] = pm.enterRecord();
            pm.addRecord(service, mail, password);
            std::cout << "Record added successfully!" << std::endl;
        } else if (choice == 2) {
            pm.showRecords();
        } else if (choice == 3) {
            if (!pm.isEmpty()) menuForFindRecord(pm);
            else std::cout << "No records to search. Please add records first." << std::endl;
        } else if (choice == 4) {
            pm.deleteAllRecords();
        } else if (choice == 5) {
            std::cout << "Exiting program. Goodbye!" << std::endl;
            return 0;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

int main() {
    const std::string master_key_file = "src/data/master_key.txt";

    if (!FileManager::fileExists(master_key_file)) {
        if (MasterKeyManager::firstRun() != 0) return 1;
    } else {
        if (!MasterKeyManager::checkMasterKey()) return 1;
    }

    PasswordManager pm;
    return mainMenu(pm);
}