#ifndef PASSWORD_RECORD_H
#define PASSWORD_RECORD_H

#include <string>

struct PasswordRecord {
    std::string service;
    std::string mail;
    std::string password;
};

#endif // PASSWORD_RECORD_H