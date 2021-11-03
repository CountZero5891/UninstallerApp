#include "regappfunc.h"

extern std::vector<RegApplication> regApp;

struct RegAppCmp {
    bool operator () (const RegApplication& regApps1, const RegApplication& regApps2)const {
        return regApps1._DisplayName < regApps2._DisplayName;
    }
};

void _call_user_apps(HWND& hList)
{
    HKEY startHKey = HKEY_CURRENT_USER;
    HKEY hUninstKey = NULL;
    HKEY hAppKey = NULL;
    WCHAR sAppKeyName[1024];
    WCHAR sUninstallPath[1024];
    WCHAR sDisplayName[1024];
    WCHAR sSubKey[1024];
    //WCHAR sDisplayName[1024];

    WCHAR sRoot[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    long lResult = ERROR_SUCCESS;
    DWORD dwType = KEY_READ;
    DWORD dwByte = KEY_ALL_ACCESS;
    DWORD dwBufferSize1 = 0;
    DWORD dwBufferSize2 = 0;

    //Open the "Uninstall" key.
    if (RegOpenKeyEx(startHKey, sRoot, 0, KEY_ALL_ACCESS, &hUninstKey) == ERROR_SUCCESS)
    {
        for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
        {
            //Enumerate all sub keys...
            dwBufferSize1 = sizeof(sAppKeyName);
            if ((lResult = RegEnumKeyEx(hUninstKey, dwIndex, (LPWSTR)sAppKeyName,
                &dwBufferSize1, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
            {
                //Open the sub key.
                wsprintf(sSubKey, L"%s\\%s", sRoot, sAppKeyName);

                if (RegOpenKeyEx(startHKey, sSubKey, 0, KEY_ALL_ACCESS, &hAppKey) != ERROR_SUCCESS)
                {
                    RegCloseKey(hAppKey);
                    RegCloseKey(hUninstKey);
                    //return false;
                }

                //Get the display name value from the application's sub key.
                dwBufferSize1 = sizeof(sDisplayName);
                if (RegQueryValueEx(hAppKey, L"DisplayName", NULL,
                    &dwType, (unsigned char*)sDisplayName, &dwBufferSize1) == ERROR_SUCCESS)
                {
                    dwBufferSize1 = sizeof(sUninstallPath);
                    if (RegQueryValueEx(hAppKey, L"UninstallString", NULL,
                        &dwType, (unsigned char*)sUninstallPath, &dwBufferSize1) == ERROR_SUCCESS)
                    {
                        regApp.push_back(RegApplication(sUninstallPath, sDisplayName, sAppKeyName, startHKey, dwByte));
                    }
                }
                RegCloseKey(hAppKey);
            }
        }

    }


    RegCloseKey(hUninstKey);
}

void _call_64_bit(HWND& hList)
{
    HKEY startHKey = HKEY_LOCAL_MACHINE;
    HKEY hUninstKey = NULL;
    HKEY hAppKey = NULL;
    WCHAR sAppKeyName[1024];
    WCHAR sUninstallPath[1024];
    WCHAR sDisplayName[1024];
    WCHAR sSubKey[1024];
    //WCHAR sDisplayName[1024];

    WCHAR sRoot[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    long lResult = ERROR_SUCCESS;
    DWORD dwType = KEY_READ;
    DWORD dwByte = KEY_ALL_ACCESS | KEY_WOW64_64KEY;
    DWORD dwBufferSize1 = 0;
    DWORD dwBufferSize2 = 0;

    //Open the "Uninstall" key.
    if (RegOpenKeyEx(startHKey, sRoot, 0, KEY_READ | KEY_WOW64_64KEY, &hUninstKey) == ERROR_SUCCESS)
    {
        for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
        {
            //Enumerate all sub keys...
            dwBufferSize1 = sizeof(sAppKeyName);
            if ((lResult = RegEnumKeyEx(hUninstKey, dwIndex, (LPWSTR)sAppKeyName,
                &dwBufferSize1, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
            {
                //Open the sub key.
                wsprintf(sSubKey, L"%s\\%s", sRoot, sAppKeyName);

                if (RegOpenKeyEx(startHKey, sSubKey, 0, KEY_READ | KEY_WOW64_64KEY, &hAppKey) != ERROR_SUCCESS)
                {
                    RegCloseKey(hAppKey);
                    RegCloseKey(hUninstKey);
                    //return false;
                }

                //Get the display name value from the application's sub key.
                dwBufferSize1 = sizeof(sDisplayName);
                if (RegQueryValueEx(hAppKey, L"DisplayName", NULL,
                    &dwType, (unsigned char*)sDisplayName, &dwBufferSize1) == ERROR_SUCCESS)
                {
                    dwBufferSize1 = sizeof(sUninstallPath);
                    if (RegQueryValueEx(hAppKey, L"UninstallString", NULL,
                        &dwType, (unsigned char*)sUninstallPath, &dwBufferSize1) == ERROR_SUCCESS)
                    {
                        regApp.push_back(RegApplication(sUninstallPath, sDisplayName, sAppKeyName, startHKey, dwByte));
                    }
                }
                RegCloseKey(hAppKey);
            }
        }

    }

    
    RegCloseKey(hUninstKey);
}

void _call_32_bit(HWND& hListBox)
{
    //setlocale(LC_ALL, "Russian");
    HKEY startHKey = HKEY_LOCAL_MACHINE;
    HKEY hUninstKey = NULL;
    HKEY hAppKey = NULL;
    WCHAR sAppKeyName[1024];
    WCHAR sSubKey[1024];
    WCHAR sDisplayName[1024];
    WCHAR sUninstallPath[1024];
    WCHAR sRoot[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    long lResult = ERROR_SUCCESS;
    DWORD dwType = KEY_READ;
    DWORD dwByte = KEY_ALL_ACCESS | KEY_WOW64_32KEY;
    DWORD dwBufferSize = 0;

    //Open the "Uninstall" key.
    if (RegOpenKeyEx(startHKey, sRoot, 0, KEY_READ | KEY_WOW64_32KEY, &hUninstKey) == ERROR_SUCCESS)
    {
        for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
        {
            //Enumerate all sub keys...
            dwBufferSize = sizeof(sAppKeyName);
            if ((lResult = RegEnumKeyEx(hUninstKey, dwIndex, sAppKeyName,
                &dwBufferSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
            {
                //Open the sub key.
                wsprintf(sSubKey, L"%s\\%s", sRoot, sAppKeyName);
                if (RegOpenKeyEx(startHKey, sSubKey, 0, KEY_READ | KEY_WOW64_32KEY, &hAppKey) != ERROR_SUCCESS)
                {
                    RegCloseKey(hAppKey);
                    RegCloseKey(hUninstKey);
                }

                //Get the display name value from the application's sub key.
                dwBufferSize = sizeof(sDisplayName);
                if (RegQueryValueEx(hAppKey, L"DisplayName", NULL,
                    &dwType, (unsigned char*)sDisplayName, &dwBufferSize) == ERROR_SUCCESS)
                {
                    dwBufferSize = sizeof(sUninstallPath);
                    if (RegQueryValueEx(hAppKey, L"UninstallString", NULL,
                        &dwType, (unsigned char*)sUninstallPath, &dwBufferSize) == ERROR_SUCCESS)
                    {

                        regApp.push_back(RegApplication(sUninstallPath, sDisplayName, sAppKeyName, startHKey, dwByte));

                    }
                }

                RegCloseKey(hAppKey);
            }
        }
    }

    

    RegCloseKey(hUninstKey);
}

void _delete_app_from_registry(std::wstring& reg_key_name, DWORD& dWord, HKEY StartKey)
{
    HKEY hKey;

    std::wstring path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + reg_key_name;

    if (RegOpenKeyExW(StartKey, path.c_str(), 0, dWord, &hKey) == ERROR_SUCCESS)
    {
        if (RegDeleteKeyExW(StartKey, path.c_str(), dWord, NULL) == ERROR_SUCCESS)
        {
            MessageBox(NULL, L"Программа удалена из реестра", L"Сообщение", MB_OK);
        }
        else {
            MessageBox(NULL, L"Произошла ошибка!", L"Сообщение", MB_OK);
        }
    }
}

void _uninstall_application(std::wstring uninstall_string)
{

    //at least this must work
    size_t count = uninstall_string.size();
    size_t pos = 0;
    pos = uninstall_string.find(L".exe");
    std::wstring execFile = uninstall_string.substr(0, pos + 5);
    std::wstring parameter = L"";
    if (count < pos + 5)
    {
        parameter = L"";
    }
    else {
        parameter = uninstall_string.substr(pos + 5);
    }
    
    SHELLEXECUTEINFO si = { 0 };
    si.cbSize = sizeof(SHELLEXECUTEINFO);
    si.fMask = SEE_MASK_NOCLOSEPROCESS;
    si.hwnd = NULL;
    si.lpFile = execFile.c_str();
    si.lpParameters = parameter.c_str();
    si.lpDirectory = NULL;
    si.nShow = SW_SHOW;
    si.hInstApp = NULL;
    ShellExecuteEx(&si);
    WaitForSingleObject(si.hProcess, INFINITE);
    CloseHandle(si.hProcess);

}

//It's deprecated, must me fixed(probably)
void _uninstall_app(std::wstring& uninstal_string)
{
    const wchar_t* input = uninstal_string.c_str();
    size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
    char* buffer = new char[size];
    size_t convertedSize;
    wcstombs_s(&convertedSize, buffer, size, input, size);
    system(buffer);
    //or maybe this, but it cant launch some executable files
    // 
    /*LPWSTR ws = new wchar_t[uninstal_string.size() + 1];
    std::copy(uninstal_string.begin(), uninstal_string.end(), ws);
    ws[uninstal_string.size()] = 0;
    wchar_t cmd[] = L"cmd.exe";
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    DWORD dwProcessId = 0;
    DWORD dwThreadId = 0;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    BOOL bCreateProcess = NULL;
    CreateProcess(NULL, ws, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);*/
   //////////////////////////////////////////////////////////////////

}

void _rename_app_in_registry(std::wstring& set_value, std::wstring reg_key_name, DWORD dwByte, HKEY StartKey)
{
    HKEY hKey = NULL;
    std::wstring value = L"DisplayName";
    std::wstring reg_path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + reg_key_name;
    if (RegOpenKeyExW(StartKey, reg_path.c_str(), 0, /*KEY_ALL_ACCESS*/dwByte, &hKey) == ERROR_SUCCESS)
    {
        ULONG bRes = RegSetValueExW(hKey, value.c_str(), 0, REG_SZ, (LPBYTE)(set_value.c_str()), (set_value.size() + 1) * sizeof(wchar_t));
        if (bRes == ERROR_SUCCESS)
        {
            MessageBox(NULL, L"Success", L"Messga", MB_OK | MB_OKCANCEL);
        }
        else {
            MessageBox(NULL, L"Fail", L"Messga", MB_OK | MB_OKCANCEL);
        }

    }
}

void _output_vector(HWND& hListBox)
{
    sort(regApp.begin(), regApp.end(), RegAppCmp());

    for (auto const& element : regApp)
    {
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)element._DisplayName.c_str());
    }
}