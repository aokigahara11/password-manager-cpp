#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Validator {
public:
    static bool CheckLetter(const std::string& line);
    static bool IsValidService(const std::string& service);
    static bool IsValidEmail(const std::string mail);
    static bool IsVaildPasswordLength(int length);
    static bool IsVaildPassword(const std::string& password);
};

#endif // VALIDATOR_H
