#include <utils/input_reader.h>
#include <ui/ui.h>
#include "password_manager/password_manager.h"
#include "password_manager/master_key_manager.h"
#include "utils/password_generator.h"
#include "password_manager/file_manager.h"
#include "utils/validator.h"
#include <iostream>
#include <limits>
#include <string>

void UI::MainMenu(PasswordManager& pm) {
    srand(static_cast<unsigned int>(time(0))); // Инициализация генератора случайных чисел

    while (true) {
        int choice;
        std::cout << "\nMain menu\n1 - Add\n2 - Show\n3 - Find\n4 - Delete all\n5 - Exit\nEnter your choice: ";
        
        // Цикл валидации ввода пользователя
        if (!InputReader::readIntInRange(choice, 1, 5)) {
            while (!InputReader::readIntInRange(choice, 1, 5)) {
                std::cout << "Please enter a valid choice (1-5): ";
            }
        }

        // Обработка выбора пользователя
        if (choice == 1) {
            auto [service, mail, password] = pm.enterRecord();
            pm.addRecord(service, mail, password);
            std::cout << "Record added successfully!" << std::endl;
        } else if (choice == 2) {
            pm.showRecords();
        } else if (choice == 3) {
            if (!pm.isEmpty()) {
                UI::menuForFindRecord(pm);
            } else {
                std::cout << "No records to search. Please add records first." << std::endl;
            }
        } else if (choice == 4) {
            pm.deleteAllRecords();
        } else if (choice == 5) {
            std::cout << "Exiting program. Goodbye!" << std::endl;
            break;
        }
    }
}

int UI::menuForFindRecord(PasswordManager& pm) {
    int choice;
    std::cout << "1 - Update record\n2 - Delete record\n3 - Back to main menu\n";
    
    if (!InputReader::readIntInRange(choice, 1, 3)) {
        while (!InputReader::readIntInRange(choice, 1, 3)) {
            std::cout << "Please enter a valid choice (1-3): ";
        }
    }

    auto [foundService, foundMail, foundPass, index] = pm.findRecord();
    if (index < 0) return 0;

    if (choice == 1) {
        std::string choice_for_update;
        std::cout << "Choice parameter for update (SERVICE/MAIL/PASS/ALL): " << std::endl;
        std::cin >> choice_for_update;
        
        // Service
        if (choice_for_update == "SERVICE" || choice_for_update == "service") {
            std::string new_service;
            std::cout << "Enter new service: ";
            
            if (InputReader::readLine(new_service)) {
                while (!Validator::IsValidService(new_service)) {
                    std::cout << "Enter new service: ";
                    InputReader::readLine(new_service);
                }
            } else {
                std::cout << "Error reading input. Update cancelled." << std::endl;
                return 0;
            }

            pm.updateRecord(index, "SERVICE", new_service);
        
        // Mail
        } else if (choice_for_update == "MAIL" || choice_for_update == "mail") {
            std::string new_mail;
            std::cout << "Enter new mail: ";

            if (InputReader::readLine(new_mail)) {
                while (!Validator::IsValidEmail(new_mail)) {
                    std::cout << "Enter new mail: ";
                    InputReader::readLine(new_mail);
                }
            } else {
                std::cout << "Error reading input. Update cancelled." << std::endl;
                return 0;
            }

            pm.updateRecord(index, "MAIL", new_mail);

        // Password
        } else if (choice_for_update == "PASS" || choice_for_update == "pass") {
            int length_password = 0;
            std::cout << "Enter length of new password: ";
        
            while (!InputReader::readInt(length_password) || !Validator::IsVaildPasswordLength(length_password)) {
                std::cout << "Enter length of new password (8-128): ";
            }
    
            std::string new_pass = PasswordGenerator::generatePassword(length_password);
            pm.updateRecord(index, "PASS", new_pass);
            std::cout << "New password generated and saved." << std::endl;
        
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
        return 0;
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
    }

    return 0;
}