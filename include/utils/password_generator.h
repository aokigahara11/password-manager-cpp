#ifndef PASSWORD_GENERATOR_H
#define PASSWORD_GENERATOR_H

#include <string>

class PasswordGenerator {
public:
    static std::string generatePassword(int length);
};

#endif // PASSWORD_GENERATOR_H