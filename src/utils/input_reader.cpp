#include <utils/input_reader.h>
#include <iostream>
#include <limits>
#include <string>

// Строка
bool InputReader::readLine(std::string& output) {
    if (!std::getline(std::cin >> std::ws, output)) {
        std::cout << "Failed to read line." << std::endl;
        return false;
    }
    return true;
}

// Целое число
bool InputReader::readInt(int& value) {
    std::cin >> value;
    if (!std::cin) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Failed to read integer." << std::endl;
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

// Целое число в диапазоне
bool InputReader::readIntInRange(int& value, int min, int max) {
    if (!readInt(value)) return false;

    if (value < min || value > max) {
        std::cout << "Value must be between " << min << " and " << max << "." << std::endl;
        return false;
    }

    return true;
}

// Символ
bool InputReader::readChar(char& value) {
    std::cin >> value;
    if (!std::cin) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Failed to read character." << std::endl;
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}