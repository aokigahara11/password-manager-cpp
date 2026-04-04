#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <tuple>
#include <string>
#include <sqlite3/sqlite3.h>

class PasswordManager {
public:
    PasswordManager(const std::string& db_path = "../src/data/passwords.db");
    ~PasswordManager();

    void addRecord(const std::string& service, const std::string& mail, const std::string& password);
    void showRecords() const;
    void deleteAllRecords();
    void deleteRecord(int id);
    std::tuple<std::string, std::string, std::string, int> findRecord();
    std::tuple<std::string, std::string, std::string> enterRecord();
    void updateRecord(int id, const std::string& field, const std::string& newValue);
    bool isEmpty() const;

private:
    sqlite3* db_;
    std::string db_path_;

    bool openDatabase();
    bool createTable();
    bool executeNonQuery(const std::string& sql) const;
    int countRecords() const;
    void showRecord(int id, const std::string& service, const std::string& mail, const std::string& password) const;
};

#endif // PASSWORD_MANAGER_H