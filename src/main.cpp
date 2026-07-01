#include "password_manager/password_manager.h"
#include "app.h"
#include <iostream>
#include <string>

int main() {
    const std::string db_path = "../src/data/passwords.db";
    PasswordManager pm(db_path);

    if (!RunImGuiApp(pm)) {
        std::cerr << "Failed to launch GUI application." << std::endl;
        return 1;
    }

    return 0;
}