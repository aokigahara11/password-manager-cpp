#include "utils/password_generator.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

std::string PasswordGenerator::generatePassword(int length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    std::string password;
    for (int i = 0; i < length; ++i) {
        password += chars[rand() % chars.size()];
    }
    std::cout << "Generated password: " << password << std::endl;
    return password;
}