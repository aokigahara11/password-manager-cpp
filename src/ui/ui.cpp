#include <ui/ui.h>
#include <utils/password_generator.h>
#include <utils/validator.h>
#include <utils/clipboard_manager.h>
#include "../third_party/imgui/imgui.h"
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cctype>

// ===================== IMGUI APPLICATION =====================

ImGuiApp::ImGuiApp(PasswordManager& pm) : m_pm(pm) {
    // Инициализируем буферы ввода нулями
    memset(service_input, 0, sizeof(service_input));
    memset(email_input, 0, sizeof(email_input));
    memset(password_input, 0, sizeof(password_input));
    memset(search_input, 0, sizeof(search_input));
    memset(master_key_input, 0, sizeof(master_key_input));
    generated_password_preview.clear();
    activity_log.push_back("Application started");
}

ImGuiApp::~ImGuiApp() {
}

void ImGuiApp::setupImGuiStyle() {
    ImGui::StyleColorsDark();
    
    // Настройка пользовательской темы и цветов
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    // Цвет фона окон
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.20f, 0.35f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.15f, 0.30f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.20f, 0.35f, 1.00f);
    
    // Цвета кнопок
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.35f, 0.60f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.25f, 0.50f, 1.00f);
    
    // Цвета заголовков
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.35f, 0.60f, 0.76f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.45f, 0.75f, 0.86f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.25f, 0.50f, 0.90f);
    
    // Цвета полей ввода
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.10f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.15f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.25f, 0.45f, 1.00f);
    
    // Цвета строки заголовка окна
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.15f, 0.25f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.25f, 0.45f, 1.00f);
    
    // Цвета текста - белый
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.95f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.60f, 0.75f, 1.00f);
    
    // Цвета разделителей
    colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.40f, 0.65f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.50f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.35f, 0.60f, 0.90f, 1.00f);
    
    // Цвета границ
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.35f, 0.60f, 0.50f);
    
    // Скругления
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
}

void ImGuiApp::appendLog(const std::string& message) {
    // Добавляем заметку в лог действий, если строка не пуста
    if (message.empty()) return;
    activity_log.push_back(message);
    if (activity_log.size() > 20) { // Только первые 20 сообщений
        activity_log.erase(activity_log.begin());
    }
}

void ImGuiApp::refreshRecords() {
    // Загружаем записи из базы данных заново
    records = m_pm.getRecords();
}

bool ImGuiApp::recordMatchesFilter(const PasswordManager::Record& record, const std::string& lower_query) const {
    std::string lower_service = record.service;
    std::string lower_mail = record.mail;
    std::string lower_pass = record.password;

    std::transform(lower_service.begin(), lower_service.end(), lower_service.begin(), ::tolower);
    std::transform(lower_mail.begin(), lower_mail.end(), lower_mail.begin(), ::tolower);
    std::transform(lower_pass.begin(), lower_pass.end(), lower_pass.begin(), ::tolower);

    return lower_service.find(lower_query) != std::string::npos ||
           lower_mail.find(lower_query) != std::string::npos ||
           lower_pass.find(lower_query) != std::string::npos;
}

void ImGuiApp::filterRecords(const std::string& query) {
    if (query.empty()) {
        refreshRecords();
        manage_message = "Showing all records";
        return;
    }

    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    std::vector<PasswordManager::Record> filtered;

    for (const auto& record : m_pm.getRecords()) {
        if (recordMatchesFilter(record, lower_query)) {
            filtered.push_back(record);
        }
    }

    records = std::move(filtered);
    manage_message = "Filter applied";
}

void ImGuiApp::renderAuthenticationWindow() {
    // Окно авторизации: ввод мастер-ключа перед показом основного UI
    if (!show_auth_window) return;

    ImGui::SetNextWindowPos(ImVec2(300, 220), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 260), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Authentication", &show_auth_window, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Password Manager Secure Access");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextWrapped("Enter your master key to continue.");
        ImGui::InputText("Master key", master_key_input, IM_ARRAYSIZE(master_key_input), ImGuiInputTextFlags_Password);

        if (!auth_message.empty()) {
            ImGui::TextColored(ImVec4(0.95f, 0.7f, 0.2f, 1.0f), "%s", auth_message.c_str());
        }

        ImGui::Spacing();
        if (ImGui::Button("Unlock", ImVec2(220, 35))) {
            std::string key(master_key_input);
            if (m_pm.authenticate(key)) {
                authenticated = true;
                show_auth_window = false;
                appendLog("Access granted");
                refreshRecords();
            } else {
                auth_message = "Incorrect master key. Try again.";
                appendLog("Failed authentication attempt");
            }
            memset(master_key_input, 0, sizeof(master_key_input));
        }
        ImGui::SameLine();
        if (ImGui::Button("Exit", ImVec2(220, 35))) {
            quit_requested = true;
        }
    }
    ImGui::End();
}

void ImGuiApp::renderAddRecordPanel() {
    // Панель добавления новой записи
    ImGui::BeginChild("AddPanel", ImVec2(0, 320), true);
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Add New Record");
    ImGui::Separator();

    ImGui::Text("Service name:");
    ImGui::InputText("##service", service_input, IM_ARRAYSIZE(service_input));

    ImGui::Text("Email:");
    ImGui::InputText("##email", email_input, IM_ARRAYSIZE(email_input));

    ImGui::Text("Password source:");
    int password_mode = generate_password ? 1 : 0;
    ImGui::RadioButton("Generate", &password_mode, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Enter manually", &password_mode, 0);
    generate_password = (password_mode == 1);

    if (generate_password) {
        ImGui::Text("Password length:");
        ImGui::SliderInt("##pwlen", &password_length, 8, 128);
        if (ImGui::Button("Generate New Password")) {
            generated_password_preview = PasswordGenerator::generatePassword(password_length);
        }
        if (generated_password_preview.empty()) {
            generated_password_preview = PasswordGenerator::generatePassword(password_length);
        }
        ImGui::InputText("Generated password", const_cast<char*>(generated_password_preview.c_str()), generated_password_preview.size() + 1, ImGuiInputTextFlags_ReadOnly);
    } else {
        ImGui::Text("Password:");
        ImGui::InputText("##password", password_input, IM_ARRAYSIZE(password_input));
    }

    ImGui::Spacing();
    if (ImGui::Button("Save Record", ImVec2(220, 35))) {
        std::string service_str(service_input);
        std::string email_str(email_input);
        std::string pwd = generate_password ? generated_password_preview : std::string(password_input);

        if (service_str.empty() || email_str.empty() || pwd.empty()) {
            appendLog("Failed to save: fill all fields");
        } else if (!Validator::IsValidService(service_str) || !Validator::IsValidEmail(email_str)) {
            appendLog("Failed to save: invalid service or email");
        } else {
            m_pm.addRecord(service_str, email_str, pwd);
            appendLog("Saved record for " + service_str);
            refreshRecords();
            memset(service_input, 0, sizeof(service_input));
            memset(email_input, 0, sizeof(email_input));
            memset(password_input, 0, sizeof(password_input));
            password_length = 16;
            generated_password_preview.clear();
        }
    }
    ImGui::EndChild();
}

void ImGuiApp::renderRecordsPanel() {
    // Панель со списком сохранённых записей
    ImGui::BeginChild("RecordsPanel", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Saved Records");
    ImGui::Separator();

    if (records.empty()) {
        ImGui::Text("No records yet.");
    } else {
        if (ImGui::BeginTable("RecordsTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Service");
            ImGui::TableSetupColumn("Email");
            ImGui::TableSetupColumn("Password");
            ImGui::TableHeadersRow();

            for (const auto& record : records) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(std::to_string(record.id).c_str(), selected_record_id == record.id)) {
                    selected_record_id = record.id;
                    strncpy_s(selected_service_input, sizeof(selected_service_input), record.service.c_str(), _TRUNCATE);
                    strncpy_s(selected_email_input, sizeof(selected_email_input), record.mail.c_str(), _TRUNCATE);
                    strncpy_s(selected_password_input, sizeof(selected_password_input), record.password.c_str(), _TRUNCATE);
                    manage_message = "Record selected";
                }
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(record.service.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(record.mail.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(record.password.c_str());
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                    ClipboardManager::copyToClipboard(record.password);
                    appendLog("Password copied for " + record.service);
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

void ImGuiApp::renderManagePanel() {
    // Панель управления: поиск, обновление, удаление и логи
    ImGui::BeginChild("ManagePanel", ImVec2(0, 0), true);
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Search & Manage");
    ImGui::Separator();
    ImGui::Text("Filter records by service, email, or password:");
    ImGui::InputText("##search_filter", search_input, IM_ARRAYSIZE(search_input));
    ImGui::SameLine();
    if (ImGui::Button("Filter", ImVec2(90, 0))) {
        filterRecords(std::string(search_input));
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        refreshRecords();
        memset(search_input, 0, sizeof(search_input));
        manage_message = "List reset";
    }

    if (selected_record_id > 0) {
        ImGui::Text("Selected record ID: %d", selected_record_id);
        ImGui::InputText("Service", selected_service_input, IM_ARRAYSIZE(selected_service_input));
        ImGui::InputText("Email", selected_email_input, IM_ARRAYSIZE(selected_email_input));
        ImGui::InputText("Password", selected_password_input, IM_ARRAYSIZE(selected_password_input));
        if (ImGui::Button("Copy Password")) {
            ClipboardManager::copyToClipboard(std::string(selected_password_input));
            appendLog("Password copied to clipboard");
        }
        ImGui::SameLine();
        if (ImGui::Button("Update Record")) {
            if (m_pm.updateRecord(selected_record_id, "SERVICE", std::string(selected_service_input)) &&
                m_pm.updateRecord(selected_record_id, "MAIL", std::string(selected_email_input)) &&
                m_pm.updateRecord(selected_record_id, "PASS", std::string(selected_password_input))) {
                refreshRecords();
                manage_message = "Record updated";
                appendLog("Record updated");
            } else {
                manage_message = "Update failed";
                appendLog("Failed to update record");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete Record")) {
            if (m_pm.deleteRecord(selected_record_id)) {
                selected_record_id = -1;
                memset(selected_service_input, 0, sizeof(selected_service_input));
                memset(selected_email_input, 0, sizeof(selected_email_input));
                memset(selected_password_input, 0, sizeof(selected_password_input));
                refreshRecords();
                manage_message = "Record deleted";
                appendLog("Record deleted");
            } else {
                manage_message = "Delete failed";
                appendLog("Failed to delete record");
            }
        }
    } else {
        ImGui::Text("Select a record to update or delete it.");
    }
    if (!manage_message.empty()) {
        ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.3f, 1.0f), "%s", manage_message.c_str());
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Activity");
    if (ImGui::BeginChild("LogChild", ImVec2(0, 220), true)) {
        ImGui::PushTextWrapPos(0.0f);
        for (const auto& entry : activity_log) {
            ImGui::TextWrapped(entry.c_str());
        }
        ImGui::PopTextWrapPos();
    }
    ImGui::EndChild();
    ImGui::EndChild();
}

void ImGuiApp::renderDashboard() {
    // Главный экран: окно на весь экран с двумя панелями
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    if (ImGui::Begin("Password Manager", &show_main_window, window_flags)) {
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            refreshRecords();
            appendLog("Records refreshed");
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete All")) {
            m_pm.deleteAllRecords();
            refreshRecords();
            appendLog("All records deleted");
        }
        ImGui::SameLine();
        if (ImGui::Button("Exit")) {
            quit_requested = true;
        }
        ImGui::Separator();

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Add Record")) {
                    appendLog("Add Record panel selected");
                }
                if (ImGui::MenuItem("Search & Manage")) {
                    appendLog("Search panel selected");
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::BeginChild("LeftPane", ImVec2(620, 0), true);
        renderAddRecordPanel();
        ImGui::Spacing();
        renderManagePanel();
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("RightPane", ImVec2(0, 0), true);
        renderRecordsPanel();
        ImGui::EndChild();
    }
    ImGui::End();
}

bool ImGuiApp::run() {
    if (quit_requested) return false;

    if (!authenticated) {
        renderAuthenticationWindow();
        return !quit_requested;
    }

    renderDashboard();
    return !quit_requested;
}


