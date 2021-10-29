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
    std::replace(uninstall_string.begin(), uninstall_string.end(), ' ', '\0');
    std::wstring str1 = uninstall_string;
    std::wstring str2 = uninstall_string;
    ShellExecute(NULL, L"open", uninstall_string.c_str(), NULL, NULL, SW_MINIMIZE);
    //CreateProcess();
    //ShellExecute(NULL, L"open", L"msiexec.exe", L"/I{A8F42E56-8D1F-4080-BD79-8375D3AD18BE}", /*L"/I{A8F42E56-8D1F-4080-BD79-8375D3AD18BE}"*/ NULL, SW_MINIMIZE);

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
    //ShellExecute(NULL, L"open", L"msiexec" L"/I{FFB40224-64C0-4D82-ADC4-6B9434B90800}", NULL, SW_MINIMIZE);
    //ShellExecute(NULL, L"open", L"msiexec.exe", L"/I{A8F42E56-8D1F-4080-BD79-8375D3AD18BE}", /*L"/I{A8F42E56-8D1F-4080-BD79-8375D3AD18BE}"*/ NULL, SW_MINIMIZE);
    //ShellExecute(NULL, L"open", L"msiexec", L"/I{A8F42E56-8D1F-4080-BD79-8375D3AD18BE}", NULL, SW_MINIMIZE);

    //ShellExecute(NULL, L"open", uninstal_string.c_str(), NULL, NULL, SW_SHOW);

}

void _rename_app_in_registry(std::wstring& set_value, std::wstring reg_key_name, DWORD dwByte, HKEY StartKey)
{
    HKEY hKey = NULL;
    std::wstring value = L"DisplayName";
    std::wstring reg_path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + reg_key_name;
    if (RegOpenKeyExW(StartKey, reg_path.c_str(), 0, /*KEY_ALL_ACCESS*/dwByte, &hKey) == ERROR_SUCCESS)
    {
        ULONG bRes = RegSetValueExW(StartKey, value.c_str(), 0, REG_SZ, (LPBYTE)(set_value.c_str()), (set_value.size() + 1) * sizeof(wchar_t));
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