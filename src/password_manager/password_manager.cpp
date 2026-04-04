#include "password_manager/password_manager.h"
#include "utils/password_generator.h"
#include <iostream>

PasswordManager::PasswordManager(const std::string& db_path)
    : db_(nullptr), db_path_(db_path) {
    if (!openDatabase()) {
        std::cerr << "Failed to open database '" << db_path_ << "'." << std::endl;
    } else if (!createTable()) {
        std::cerr << "Failed to create passwords table." << std::endl;
    }
}

PasswordManager::~PasswordManager() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool PasswordManager::openDatabase() {
    int rc = sqlite3_open(db_path_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db_) << std::endl;
        if (db_) sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }
    return true;
}

bool PasswordManager::createTable() {
    const char* sql = "CREATE TABLE IF NOT EXISTS passwords ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "service TEXT NOT NULL, "
                      "mail TEXT NOT NULL, "
                      "password TEXT NOT NULL" 
                      ");";
    return executeNonQuery(sql);
}

bool PasswordManager::executeNonQuery(const std::string& sql) const {
    if (!db_) return false;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "unknown") << std::endl;
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int PasswordManager::countRecords() const {
    if (!db_) return 0;
    const char* sql = "SELECT COUNT(*) FROM passwords;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare count statement: " << sqlite3_errmsg(db_) << std::endl;
        return 0;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

void PasswordManager::showRecord(int id, const std::string& service, const std::string& mail, const std::string& password) const {
    std::cout << id << "\t" << service << "\t" << mail << "\t" << password << std::endl;
}

void PasswordManager::addRecord(const std::string& service, const std::string& mail, const std::string& password) {
    if (!db_) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    const char* sql = "INSERT INTO passwords (service, mail, password) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db_) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, service.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, mail.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert record: " << sqlite3_errmsg(db_) << std::endl;
    }
    sqlite3_finalize(stmt);
}

void PasswordManager::showRecords() const {
    if (!db_) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    const char* sql = "SELECT id, service, mail, password FROM passwords;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db_) << std::endl;
        return;
    }

    bool any = false;
    std::cout << "ID\tService\tMail\tPassword" << std::endl;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        any = true;
        int id = sqlite3_column_int(stmt, 0);
        const char* service = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* mail = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        showRecord(id,
                   service ? service : "",
                   mail ? mail : "",
                   password ? password : "");
    }

    if (!any) {
        std::cout << "No records found." << std::endl;
    }
    sqlite3_finalize(stmt);
}

void PasswordManager::deleteAllRecords() {
    if (!executeNonQuery("DELETE FROM passwords;")) {
        std::cerr << "Failed to delete all records." << std::endl;
    } else {
        std::cout << "All records deleted successfully!" << std::endl;
    }
}

void PasswordManager::deleteRecord(int id) {
    if (!db_) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    const char* sql = "DELETE FROM passwords WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db_) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE && sqlite3_changes(db_) > 0) {
        std::cout << "Record deleted successfully!" << std::endl;
    } else {
        std::cout << "Invalid record id!" << std::endl;
    }
    sqlite3_finalize(stmt);
}

std::tuple<std::string, std::string, std::string, int> PasswordManager::findRecord() {
    if (isEmpty()) {
        std::cout << "No records to search." << std::endl;
        return {"", "", "", -1};
    }

    int choice;
    std::string request;
    std::cout << "1 - Service\n2 - Mail\n3 - Password\nChoice parameter for search: ";
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "Enter service: ";
        std::cin >> request;
    } else if (choice == 2) {
        std::cout << "Enter mail: ";
        std::cin >> request;
    } else if (choice == 3) {
        std::cout << "Enter password: ";
        std::cin >> request;
    } else {
        std::cout << "Invalid parameter choice." << std::endl;
        return {"", "", "", -1};
    }

    if (request.empty()) {
        std::cout << "Search request cannot be empty." << std::endl;
        return {"", "", "", -1};
    }

    std::string sql = "SELECT id, service, mail, password FROM passwords WHERE ";
    if (choice == 1) {
        sql += "service = ? LIMIT 1;";
    } else if (choice == 2) {
        sql += "mail = ? LIMIT 1;";
    } else {
        sql += "password = ? LIMIT 1;";
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare search statement: " << sqlite3_errmsg(db_) << std::endl;
        return {"", "", "", -1};
    }

    sqlite3_bind_text(stmt, 1, request.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* service = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* mail = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::cout << "Found record with ID " << id << ": ";
        showRecord(id,
                   service ? service : "",
                   mail ? mail : "",
                   password ? password : "");
        sqlite3_finalize(stmt);
        return {service ? service : "", mail ? mail : "", password ? password : "", id};
    }

    std::cout << "Record not found." << std::endl;
    sqlite3_finalize(stmt);
    return {"", "", "", -1};
}

std::tuple<std::string, std::string, std::string> PasswordManager::enterRecord() {
    std::string service, mail, password;
    std::cout << "Enter service: ";
    std::cin >> service;
    if (service.empty()) {
        std::cout << "Service cannot be empty." << std::endl;
        return {"", "", ""};
    }

    std::cout << "Enter mail: ";
    std::cin >> mail;
    if (mail.empty()) {
        std::cout << "Mail cannot be empty." << std::endl;
        return {"", "", ""};
    }

    int length_password;
    while (true) {
        std::cout << "Enter length of password: ";
        std::cin >> length_password;
        if (length_password >= 8) {
            break;
        }
        std::cout << "Password length must be at least 8 characters. Please try again." << std::endl;
    }

    password = PasswordGenerator::generatePassword(length_password);
    return {service, mail, password};
}

void PasswordManager::updateRecord(int id, const std::string& field, const std::string& newValue) {
    if (!db_) {
        std::cerr << "Database is not open." << std::endl;
        return;
    }

    std::string sql;
    if (field == "SERVICE") {
        sql = "UPDATE passwords SET service = ? WHERE id = ?;";
    } else if (field == "MAIL") {
        sql = "UPDATE passwords SET mail = ? WHERE id = ?;";
    } else if (field == "PASS") {
        sql = "UPDATE passwords SET password = ? WHERE id = ?;";
    } else {
        std::cout << "Invalid field." << std::endl;
        return;
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db_) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, id);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE && sqlite3_changes(db_) > 0) {
        std::cout << "Record updated successfully." << std::endl;
    } else {
        std::cout << "Invalid record id or no change made." << std::endl;
    }
    sqlite3_finalize(stmt);
}

bool PasswordManager::isEmpty() const {
    return countRecords() == 0;
}
