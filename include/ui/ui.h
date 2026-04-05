#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "password_manager/password_manager.h"

class UI {
public:
    static void MainMenu(PasswordManager& pm);
    static int menuForFindRecord(PasswordManager& pm);
};

#endif // MAIN_MENU_H