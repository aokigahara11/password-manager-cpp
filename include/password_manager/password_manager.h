#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <tuple>
#include <string>
#include <vector>

#ifndef SQLITE_HAS_CODEC
#define SQLITE_HAS_CODEC
#endif

#include <sqlite3/sqlite3.h>

class PasswordManager {
public:
    struct Record {
        int id;
        std::string service;
        std::string mail;
        std::string password;
    };

    PasswordManager(const std::string& db_path);
    ~PasswordManager();

    bool authenticate(const std::string& key);
    bool databaseExists() const;

    void addRecord(const std::string& service, const std::string& mail, const std::string& password);
    std::vector<Record> getRecords() const;
    void showRecords() const;
    void deleteAllRecords();
    bool deleteRecord(int id);
    std::tuple<std::string, std::string, std::string, int> findRecord();
    std::tuple<std::string, std::string, std::string> enterRecord();
    bool updateRecord(int id, const std::string& field, const std::string& newValue);
    bool isEmpty() const;

private:
    sqlite3* db_;
    std::string db_path_;
    
    bool openDatabase(const std::string& key);
    bool createTable();
    bool executeNonQuery(const std::string& sql) const;
    int countRecords() const;
    void showRecord(int id, const std::string& service, const std::string& mail, const std::string& password) const;
};

#endif // PASSWORD_MANAGER_H