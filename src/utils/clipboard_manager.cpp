#include "utils/clipboard_manager.h"
#include <iostream>
#include <windows.h>
#include <cstring>

bool ClipboardManager::copyToClipboard(const std::string& text) {
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Not try open clipboard." << std::endl;
        return false;
    }

    if (!EmptyClipboard()) {
        std::cerr << "Not try clear clipboard." << std::endl;
        CloseClipboard();
        return false;
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hMem) {
        std::cerr << "Error allocating memory.\n";
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
        std::cerr << "Ошибка записи в буфер обмена.\n";
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}