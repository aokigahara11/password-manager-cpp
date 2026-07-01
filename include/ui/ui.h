#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "password_manager/password_manager.h"
#include <string>
#include <vector>

class ImGuiApp;

// ImGui Application Manager
class ImGuiApp {
public:
    ImGuiApp(PasswordManager& pm);
    ~ImGuiApp();
    
    bool run();
    
private:
    PasswordManager& m_pm;
    
    // UI state
    bool authenticated = false;
    bool quit_requested = false;
    bool show_auth_window = true;
    bool show_main_window = true;
    bool generate_password = true;
    int selected_record_id = -1;
    
    char master_key_input[256] = "";
    char service_input[128] = "";
    char email_input[128] = "";
    char password_input[256] = "";
    char selected_service_input[128] = "";
    char selected_email_input[128] = "";
    char selected_password_input[256] = "";
    char search_input[128] = "";
    int password_length = 16;
    std::string auth_message;
    std::string generated_password_preview;
    std::string manage_message;
    std::vector<PasswordManager::Record> records;
    std::vector<std::string> console_log;
    
    // UI methods
    void renderAuthenticationWindow(); // окно ввода мастер-ключа
    void renderDashboard(); // главный экран с панелями
    void renderAddRecordPanel(); // панель добавления новой записи
    void renderManagePanel(); // панель управления и фильтрации
    void renderRecordsPanel(); // панель со списком записей
    void refreshRecords(); // загрузить записи из БД заново
    void filterRecords(const std::string& query); // отфильтровать записи по запросу
    bool recordMatchesFilter(const PasswordManager::Record& record, const std::string& lower_query) const; // проверка записи на совпадение с фильтром
    void appendLog(const std::string& message); // добавить сообщение в лог действий
    
public:
    void setupImGuiStyle(); // настроить тему и цвета ImGui

private:
};

#endif // MAIN_MENU_H