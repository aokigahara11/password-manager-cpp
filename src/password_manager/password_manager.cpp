#include "password_manager/password_manager.h"
#include "utils/password_generator.h"
#include <iostream>
#include <algorithm>

void PasswordManager::addRecord(const std::string& service, const std::string& mail, const std::string& password) {
    PasswordRecord newRecord;
    newRecord.service = service;
    newRecord.mail = mail;
    newRecord.password = password;
    records_.push_back(newRecord);
}

void PasswordManager::showRecords() const {
    if (records_.empty()) {
        std::cout << "No records found." << std::endl;
        return;
    }

    std::cout << "Service" << "\t" << "Mail" << "\t" << "Password" << std::endl;
    for (const auto& record : records_) {
        showRecord(record);
    }
}

void PasswordManager::deleteAllRecords() {
    records_.clear();
    std::cout << "All records deleted successfully!" << std::endl;
}

void PasswordManager::deleteRecord(int index) {
    if (index >= 0 && index < records_.size()) {
        records_.erase(records_.begin() + index);
        std::cout << "Record deleted successfully!" << std::endl;
    } else {
        std::cout << "Invalid record index!" << std::endl;
    }
}

std::tuple<std::string, std::string, std::string, int> PasswordManager::findRecord() {
    if (records_.empty()) {
        std::cout << "No records to search." << std::endl;
        return {"", "", "", -1};
    }

    int choice;
    std::string request;

    std::cout << "1 - Service\n2 - Mail\n3 - Password\nChoice parameter for search: ";
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "Enter service: ";
        std::cin >> request;
        if (request.empty()) return {"", "", "", -1};
    } else if (choice == 2) {
        std::cout << "Enter mail: ";
        std::cin >> request;
        if (request.empty()) return {"", "", "", -1};
    } else if (choice == 3) {
        std::cout << "Enter password: ";
        std::cin >> request;
        if (request.empty()) return {"", "", "", -1};
    } else {
        std::cout << "Invalid parameter choice." << std::endl;
        return {"", "", "", -1};
    }

    for (size_t i = 0; i < records_.size(); ++i) {
        bool match = false;
        if (choice == 1 && records_[i].service == request) match = true;
        if (choice == 2 && records_[i].mail == request) match = true;
        if (choice == 3 && records_[i].password == request) match = true;

        if (match) {
            int index = static_cast<int>(i);
            std::cout << "Found at index " << i + 1 << ": ";
            showRecord(records_[i]);
            return {records_[i].service, records_[i].mail, records_[i].password, index};
        }
    }

    std::cout << "Record not found." << std::endl;
    return {"", "", "", -1};
}

std::tuple<std::string, std::string, std::string> PasswordManager::enterRecord() {
    std::string service, mail, password;
    std::cout << "Enter service: ";
    std::cin >> service;
    if (service.empty()) {
        std::cout << "Service cannot be empty." << std::endl;
        return {"", "", ""};
    }

    std::cout << "Enter mail: ";
    std::cin >> mail;

    if (mail.empty()) {
        std::cout << "Mail cannot be empty." << std::endl;
        return {"", "", ""};
    }

    int length_password;

    while (true) {
        std::cout << "Enter length of password: ";
        std::cin >> length_password;

        if (length_password >= 8) {
            break;
        }

        std::cout << "Password length must be at least 8 characters. Please try again." << std::endl;
    }

    password = PasswordGenerator::generatePassword(length_password);
    return {service, mail, password};
}

void PasswordManager::updateRecord(int index, const std::string& field, const std::string& newValue) {
    if (index < 0 || index >= records_.size()) {
        std::cout << "Invalid index." << std::endl;
        return;
    }

    if (field == "SERVICE") {
        records_[index].service = newValue;
        std::cout << "Service updated." << std::endl;
    } else if (field == "MAIL") {
        records_[index].mail = newValue;
        std::cout << "Mail updated." << std::endl;
    } else if (field == "PASS") {
        records_[index].password = newValue;
        std::cout << "Password updated." << std::endl;
    } else {
        std::cout << "Invalid field." << std::endl;
    }
}

void PasswordManager::showRecord(const PasswordRecord& record) const {
    std::cout << record.service << " " << record.mail << " " << record.password << std::endl;
}