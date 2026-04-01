// tests/sqlite3_test.cpp
#include <iostream>
#include <sqlite3/sqlite3.h>
using namespace std;

// Реализация функций:
    // Создание базы данных +
    // Создание таблицы +
    // Добавление записи +
    // Удалить запись +
    // Обновление записи +
    // Чтение всех записей +

// Константы для базы данных
sqlite3* db;
const string db_name = "test.db";

// Создание базы данных
bool createDatabase() {
    int rc = sqlite3_open(db_name.c_str(), &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return false;
    } else {
        cout << "Opened database successfully" << endl;
        return true;
    }
}

// Создание таблицы
bool createTable() {
    const char* sql = "CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, service TEXT, email TEXT, password TEXT);";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "Table created successfully" << endl;
        return true;
    }
}

// Добавление записи
bool addRecord(string service, string mail, string password) {
    string sql = "INSERT INTO test (service, email, password) VALUES ('" + service + "', '" + mail + "', '" + password + "');";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "Record added successfully" << endl;
        return true;
    }
}

// Удаление запись по id
bool deleteRecord(string id) {
    string sql = "DELETE FROM test WHERE id = " + id + ";";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "Record deleted successfully" << endl;
        return true;
    }
}

// Чтение всех записей
bool readAllRecords() {
    const char* sql = "SELECT id, service, email, password FROM test;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    cout << "Records:" << endl;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* service = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        cout << "ID: " << id << ", Service: " << service << ", Email: " << email << ", Password: " << password << endl;
    }
    if (rc != SQLITE_DONE) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

// Обновление записи по id
bool updateRecord(string id, string service, string mail, string password) {
    string sql = "UPDATE test SET service = '" + service + "', email = '" + mail + "', password = '" + password + "' WHERE id = " + id + ";";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "Record updated successfully" << endl;
        return true;
    }
}

// Тестирование функций
int main() {
    // Создание базы данных
    if (!createDatabase()) {
        return 1;
    }

    // Создание таблицы
    if (!createTable()) {
        return 1;
    }

    // Мини-меню для тестирования функций
    int choice;
    while (true) {
        cout << "1 - Add record\n2 - Delete record\n3 - Read all records\n4 - Update record\n5 - Exit\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            string service, mail, password;
            cout << "Enter service: ";
            cin >> service;
            cout << "Enter mail: ";
            cin >> mail;
            cout << "Enter password: ";
            cin >> password;
            addRecord(service, mail, password);
        } else if (choice == 2) {
            string id;
            cout << "Enter record id to delete: ";
            cin >> id;
            deleteRecord(id);
        } else if (choice == 3) {
            readAllRecords();
        } else if (choice == 4) {
            string id, service, mail, password;
            cout << "Enter id to update: ";
            cin >> id;
            cout << "Enter new service: ";
            cin >> service;
            cout << "Enter new mail: ";
            cin >> mail;
            cout << "Enter new password: ";
            cin >> password;
            updateRecord(id, service, mail, password);
        } else if (choice == 5) {
            cout << "Exiting..." << endl;
            break;
        } else {
            cout << "Invalid choice. Try again." << endl;
        }
    }
    return 0;
}