#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <vector>
#include <tuple>
#include <string>
#include "password_record.h"

class PasswordManager {
public:
    void addRecord(const std::string& service, const std::string& mail, const std::string& password);
    void showRecords() const;
    void deleteAllRecords();
    void deleteRecord(int index);
    std::tuple<std::string, std::string, std::string, int> findRecord();
    std::tuple<std::string, std::string, std::string> enterRecord();
    void updateRecord(int index, const std::string& field, const std::string& newValue);
    bool isEmpty() const { return records_.empty(); }
private:
    std::vector<PasswordRecord> records_;
    void showRecord(const PasswordRecord& record) const;
};

#endif // PASSWORD_MANAGER_H