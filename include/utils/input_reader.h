#ifndef INPUT_READER_H
#define INPUT_READER_H

#include <iostream>
#include <limits>
#include <string>

class InputReader {
public:
    static bool readLine(std::string& output);
    static bool readInt(int& value);
    static bool readIntInRange(int& value, int min, int max);
    static bool readChar(char& value);
};

#endif // INPUT_READER_H