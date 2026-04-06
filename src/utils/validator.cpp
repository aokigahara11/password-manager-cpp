#include <iostream>
#include "utils/validator.h"

// Проверка что строка содержит хотя бы одну букву
bool Validator::CheckLetter(const std::string& line) {
    int count_letter = 0;
    
    for (char c : line) {
        if (std::isalpha(c)) {
            count_letter++;
        }
    }

    if (count_letter == 0) {
        std::cout << "Line does not contain letters. Please try again." << std::endl;
        return false;
    }

    return true;
}

// Проверка валидности названия сервиса
bool Validator::IsValidService(const std::string& service) {
    // Строка не пустая
    if (service.empty()) {
        std::cout << "Service cannot be empty." << std::endl;
        return false;
    }
    
    // Строка не состоит только из пробелов
    if (service.find_first_not_of(" \t\r\n") == std::string::npos) {
        std::cout << "Service cannot be empty or whitespace only." << std::endl;
        return false;
    }

    // Лимит на длину названия сервиса
    if (service.length() > 100 ) {
        std::cout << "Service name is too long. Please limit to 100 characters." << std::endl;
        return false;
    }

    if (!CheckLetter(service)) {
        return false;
    }

    return true;
}

// Проверка валидности почты (понимаем, что пользователь может вести не почту, а логин от сервиса, поэтому не требуем наличия '@')
bool Validator::IsValidEmail(const std::string mail) {
    if (mail.length() < 3 || mail.length() > 128) {
        std::cout << "Mail must be at least 3 and at most 128 characters long. Please try again." << std::endl;
        return false;
    }

    if (!CheckLetter(mail)) {
        return false;
    }
    
    return true;
}

// Проверка валидности длины пароля (генератор)
bool Validator::IsVaildPasswordLength(int length) {
    if (length < 8 || length > 128) {
        std::cout << "Password length must be between 8 and 128 characters. Please try again." << std::endl;
        return false;
    }
    return true;
}

// Проверка валидности пароля (ввод пользователем)
bool Validator::IsVaildPassword(const std::string& password) {
    if (password.length() < 8 || password.length() > 128) {
        std::cout << "Password length must be between 8 and 128 characters. Please try again." << std::endl;
        return false;
    }

    if (!CheckLetter(password)) {
        return false;
    }
    
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;

    for (char ch : password) {
        if (std::isupper(static_cast<unsigned char>(ch))) hasUpper = true;
        else if (std::islower(static_cast<unsigned char>(ch))) hasLower = true;
        else if (std::isdigit(static_cast<unsigned char>(ch))) hasDigit = true;
        else if (std::ispunct(static_cast<unsigned char>(ch))) hasSpecial = true;
    }

    int count_warnings = hasUpper + hasLower + hasDigit + hasSpecial;

    if (count_warnings < 3) {
        std::cout << "Warning: Your password is weak. Consider using a mix of uppercase, lowercase, digits, and special characters." << std::endl;
        // Возвращаем true, так как это только предупреждение, а не ошибка
    }

    return true; // Пароль принят в любом случае, если длина корректна
}