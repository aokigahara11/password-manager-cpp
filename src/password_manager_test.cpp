// src/password_manager_test.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <filesystem>
#include <cstring>
using namespace std;

// Реализация функций:
    // Добавление записи +
    // Просмотр всех записей +
    // Поиск записи +
    // Удаление записи +
    // Удаление всех записей +
    // Обновление записи +
    // Генерация паролей +
    // Главное меню +
    // Система мастера-ключ +
    // Поддержка сохранения в буфер обмена +

// Структура для хранения записи пароля
struct PasswordsRecords {
    string service;
    string mail;
    string password;
};

// Поддержка буфера обмена (Windows)
bool Clipboard(string& text) {
    if (!OpenClipboard(nullptr)) {
        cerr << "Not try open clipboard." << endl;
        return false;
    }

    if (!EmptyClipboard()) {
        cerr << "Not try clear clipboard." << endl;
        CloseClipboard();
        return false;
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hMem) {
        cerr << "Error allocating memory.\n";
        CloseClipboard();
        return false;
    }

    char* buffer = static_cast<char*>(GlobalLock(hMem));
    if (!buffer) {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }
    memcpy(buffer, text.c_str(), text.size() + 1);
    GlobalUnlock(hMem);

    if (!SetClipboardData(CF_TEXT, hMem)) {
        cerr << "Ошибка записи в буфер обмена.\n";
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

// Создание файла с мастер-ключом
bool CreateFileMasterKey(string master_key) {
    string dir_path = "src/data";
    if (!filesystem::exists(dir_path)) {
        filesystem::create_directories(dir_path);
    }

    string file_path = dir_path + "/master_key.txt";
    ofstream outFile(file_path, ios::out);
    if (outFile.is_open()) {
        outFile << master_key;
        outFile.close();
        return true;
    }
    return false;
}

// Проверка существования файла
bool FileExists(const string& file_path) {
    return filesystem::exists(file_path);
}

// Генерация случайного пароля
string GeneratePassword(int length) {
    const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    string password;
    for (int i = 0; i < length; ++i) {
        password += chars[rand() % chars.size()];
    }
    cout << "Generated password: " << password << endl;
    return password;
}

// Показ одной записи
void ShowRecord(PasswordsRecords record) {
    cout << record.service << " " << record.mail << " " << record.password << endl;
}

// Добавление записи в вектор
void AddRecord(vector<PasswordsRecords> &records, string service, string mail, string password) {
    PasswordsRecords newRecord;
    newRecord.service = service;
    newRecord.mail = mail;
    newRecord.password = password;
    records.push_back(newRecord);
}

// Показ всех записей
void ShowRecords(vector<PasswordsRecords> &records) {
    if (records.empty()) {
        cout << "No records found." << endl;
        return;
    }

    cout << "Service" << "\t" << "Mail" << "\t" << "Password" << endl;
    for (size_t i = 0; i < records.size(); i++) {
        cout << records[i].service << " " << records[i].mail << " " << records[i].password << endl;
    }
}

// Удаление всех записей
void DeleteAllRecords(vector<PasswordsRecords> &records) {
    records.clear();
    cout << "All records deleted successfully!" << endl;
}

// Удаление записи по индексу
void DeleteRecord(vector<PasswordsRecords> &records, int index) {
    if (index >= 0 && index < records.size()) {
        records.erase(records.begin() + index);
        cout << "Record deleted successfully!" << endl;
    } else {
        cout << "Invalid record index!" << endl;
    }
}

// Поиск записи
tuple<string, string, string, int> FindRecord(vector<PasswordsRecords> &records) {
    if (records.empty()) {
        cout << "No records to search." << endl;
        return {"", "", "", -1};
    }

    int choice;
    string request;

    cout << "1 - Service\n2 - Mail\n3 - Password\nChoice parameter for search: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Enter service: ";
        cin >> request;
        if (request.empty()) return {"", "", "", -1};
    } else if (choice == 2) {
        cout << "Enter mail: ";
        cin >> request;
        if (request.empty()) return {"", "", "", -1};
    } else if (choice == 3) {
        cout << "Enter password: ";
        cin >> request;
        if (request.empty()) return {"", "", "", -1};
    } else {
        cout << "Invalid parameter choice." << endl;
        return {"", "", "", -1};
    }

    for (size_t i = 0; i < records.size(); i++) {
        bool match = false;
        if (choice == 1 && records[i].service == request) match = true;
        if (choice == 2 && records[i].mail == request) match = true;
        if (choice == 3 && records[i].password == request) match = true;

        if (match) {
            int index = (int)i;
            cout << "Found at index " << i + 1 << ": "; // Не учитываем нулевой индекс для пользователя
            ShowRecord(records[i]);
            return {records[i].service, records[i].mail, records[i].password, index};
        }
    }

    cout << "Record not found." << endl;
    return {"", "", "", -1};
}

// Ввод новой записи
tuple<string, string, string> EnterRecord() {
    string service, mail, password;
    cout << "Enter service: ";
    cin >> service;
    if (service.empty()) {
        cout << "Service cannot be empty." << endl;
        return {"", "", ""};
    }

    cout << "Enter mail: ";
    cin >> mail;
    
    if (mail.empty()) {
        cout << "Mail cannot be empty." << endl;
        return {"", "", ""};
    }

    int length_password;
    
    // Бесконечный цикл пока пользователь не введет корректную длину пароля
    while (true) {
        cout << "Enter length of password: ";
        cin >> length_password;
    
        if (length_password >= 8) {
            break;
        }
    
        cout << "Password length must be at least 8 characters. Please try again." << endl;
        }
    
    password = GeneratePassword(length_password);
    return {service, mail, password};
}

// Первоначальный запуск (создание мастер-ключа)
int FurstRun() {
    cout << "Welcome to password-manager-cpp!" << endl;
    cout << "For management need create master key." << endl;
    cout << "Enter your master key or generate? (1/2): ";
    int choice; cin >> choice;

    if (choice == 1) {
        string master_key;
        cout << "Enter your master key: ";
        cin >> master_key;
        if (!CreateFileMasterKey(master_key)) {
            cout << "Error saving master key to file." << endl;
            return 1;
        }
        if (Clipboard(master_key)) cout << "Master key copied to clipboard." << endl;
    } else if (choice == 2) {
        int length_master_key;
        cout << "Enter length of master key: ";
        cin >> length_master_key;
        string master_key = GeneratePassword(length_master_key);
        cout << "Your generated master key: " << master_key << endl;
        if (!CreateFileMasterKey(master_key)) {
            cout << "Error saving master key to file." << endl;
            return 1;
        }
        if (Clipboard(master_key)) cout << "Master key copied to clipboard." << endl;
    }
    return 0;
}

// Проверка мастер-ключа
bool CheckMasterKey() {
    string file_path = "src/data/master_key.txt";
    int attempts = 0;

    while (attempts < 5) {
        string input_key;
        cout << "Enter master key: ";
        cin >> input_key;

        ifstream inFile(file_path);
        if (inFile.is_open()) {
            string stored_key;
            getline(inFile, stored_key);
            inFile.close();

            if (input_key == stored_key) {
                cout << "Access granted!" << endl;
                return true;
            } else {
                cout << "Incorrect master key. Try again." << endl;
                attempts++;
            }

            if (attempts == 5) {
                cout << "Too many failed attempts. Exiting program." << endl;
                return false;
            }

        } else {
            cerr << "Error opening master key file." << endl;
            return false;
        }
    }
    return false;
}

// Главное меню
int MainMenu();
int MenuForFindRecord(vector<PasswordsRecords> &records);

// Меню редактирования найденной записи
int MenuForFindRecord(vector<PasswordsRecords> &records) {
    int choice;
    cout << "1 - Update record\n2 - Delete record\n3 - Back to main menu\n";
    cin >> choice;

    auto [foundservice, foundMail, foundPass, index] = FindRecord(records);
    if (index < 0) return 0;

    if (choice == 1) {
        string choice_for_update;
        cout << "Choice parameter for update (SERVICE/MAIL/PASS/ALL): " << endl;
        cin >> choice_for_update;
        if (choice_for_update == "SERVICE") {
            string new_service;
            cout << "Enter new service: ";
            cin >> new_service;
            records[index].service = new_service;
            cout << "Service updated." << endl;
        } else if (choice_for_update == "MAIL") {
            string new_mail;
            cout << "Enter new mail: ";
            cin >> new_mail;
            records[index].mail = new_mail;
            cout << "Mail updated." << endl;
        } else if (choice_for_update == "PASS") {
            int length_password;
            cout << "Enter length of new password: ";
            cin >> length_password;
            records[index].password = GeneratePassword(length_password);
            cout << "Password updated." << endl;
        } else if (choice_for_update == "ALL") {
            auto [new_service, new_mail, new_password] = EnterRecord();
            records[index].service = new_service;
            records[index].mail = new_mail;
            records[index].password = new_password;
            cout << "Record fully updated." << endl;
        } else {
            cout << "Invalid parameter choice for update." << endl;
        }
    } else if (choice == 2) {
        DeleteRecord(records, index);
    } else if (choice == 3) {
        MainMenu();
    } else {
        cout << "Invalid choice. Please try again." << endl;
    }

    return 0;
}

// Главное меню
int MainMenu() {
    vector<PasswordsRecords> records;
    srand((unsigned int)time(0));

    while (true) {
        int choice;
        cout << "\nMain menu\n1 - Add\n2 - Show\n3 - Find\n4 - Delete all\n5 - Exit\nEnter your choice: ";
        cin >> choice;

        if (choice == 1) {
            auto [service, mail, password] = EnterRecord();
            AddRecord(records, service, mail, password);
            cout << "Record added successfully!" << endl;
        } else if (choice == 2) {
            ShowRecords(records);
        } else if (choice == 3) {
            if (!records.empty()) MenuForFindRecord(records);
            else cout << "No records to search. Please add records first." << endl;
        } else if (choice == 4) {
            DeleteAllRecords(records);
        } else if (choice == 5) {
            cout << "Exiting program. Goodbye!" << endl;
            return 0;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

int main() {
    const string master_key_file = "src/data/master_key.txt";

    if (!FileExists(master_key_file)) {
        if (FurstRun() != 0) return 1;
    } else {
        if (!CheckMasterKey()) return 1;
    }

    return MainMenu();
}