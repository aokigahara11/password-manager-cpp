#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Validator {
public:
    static bool IsValidService(const std::string& service);
    static bool IsValidEmail(std::string mail);
    static bool IsVaildPasswordLength(int length);
    static bool IsVaildPassword(const std::string& password);
};

#endif // VALIDATOR_H
