
// UninstallerApp.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "UninstallerApp.h"
#include "ShellAPI.h"
#include <string>
#include <strsafe.h>
#include <vector>
#include <algorithm>

#define MAX_LOADSTRING 100
#define BUFFER 8192

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hListBox, hBtn1, hBtn2, hBtn3, hBtn4;
HWND hwndGoto = NULL;
HWND editText = NULL;
WCHAR sUninstallPath[1024];
WCHAR sDisplayName[1024];
std::vector<std::wstring> dsNm;
int iLine;             // Receives line number.
BOOL fRelative;        // Receives check box status. 

struct RegApplication 
{
    std::wstring _UninstallPath;
    std::wstring _DisplayName;
    std::wstring _RegKeyName;
    DWORD _DwType;
    RegApplication(WCHAR unPath[1023], WCHAR dispName[1023], WCHAR regKeyName[1023], DWORD dwType)
    {
        _UninstallPath = unPath;
        _DisplayName = dispName;
        _RegKeyName = regKeyName;
        _DwType = dwType;
    };
};

struct RegAppCmp {
    bool operator () (const RegApplication& regApps1, const RegApplication& regApps2)const {
        return regApps1._DisplayName < regApps2._DisplayName;
    }
};

std::vector<RegApplication> regApp;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GoToProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PasswordProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GoToProc(HWND, UINT, WPARAM, LPARAM);

void TrayDrawIcon(HWND hWnd);
void _call_64_bit(HWND& hList);
void _call_32_bit(HWND& hList);
void _output_vector(HWND& hListBox);

void _uninstall_app(std::wstring& uninstal_string);
void _delete_app_from_registry(std::wstring& reg_key_name, DWORD& dWord);
void _rename_app_in_registry(std::wstring& app_display_name, DWORD& dWord);

bool compareByLength(const RegApplication& a, const RegApplication& b);
//void _find_uninstall_string(std::wstring &uninstal_string);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.
    HANDLE Test_Present = CreateMutex(NULL, false, L"UninstallerApp");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBox(0, L"Программа уже запущена", NULL, MB_OK);
        return 0;
    }
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_UNINSTALLERAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UNINSTALLERAPP));

    MSG msg;
    BOOL bRet;
    // Цикл основного сообщения:
    /*while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }*/
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            // Handle the error and possibly exit
        }
        else if (!IsWindow(hwndGoto) || !IsDialogMessage(hwndGoto, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    CloseHandle(Test_Present);
    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNINSTALLERAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_UNINSTALLERAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, 
       szTitle, 
       WS_OVERLAPPEDWINDOW,
       600, 
       50, 
       700, 
       600, 
       nullptr, 
       nullptr, 
       hInstance, 
       nullptr);

  
   
   hListBox = CreateWindowW(L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_WANTKEYBOARDINPUT, 30, 30, 500, 500, hWnd, (HMENU)ID_LIST, hInst, NULL);
   hBtn1 = CreateWindowEx(NULL, L"button", L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 30, 70, 30, hWnd, (HMENU)ID_BTN_DELETE, hInst, NULL);
   hBtn2 = CreateWindowEx(NULL, L"button", L"Rename", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 90, 70, 30, hWnd, (HMENU)ID_BTN_RENAME, hInst, NULL);
   hBtn3 = CreateWindowEx(NULL, L"button", L"Uninstall", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 150, 70, 30, hWnd, (HMENU)ID_BTN_UNINSTALL, hInst, NULL);
   hBtn4 = CreateWindowEx(NULL, L"button", L"Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 210, 70, 30, hWnd, (HMENU)ID_BTN_EXIT, hInst, NULL);
    //hBtn4 = CreateWindowEx(NULL, L"button", L"Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 210, 70, 30, hWnd, (HMENU)ID_BTN_EXIT, hInst, NULL);

   

   if (!hWnd)
   {
      return FALSE;
   }
   
   _call_64_bit(hListBox);
    //sort(regApp.begin(), regApp.end(), RegAppCmp());
   _call_32_bit(hListBox);
   //sort(regApp.begin(), regApp.end(), RegAppCmp());
   _output_vector(hListBox);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int i = 0;
    WCHAR str_2[255];
    HWND hwndList;
    //std::string str_3 = "Sublime Text 3_is1";
    WCHAR str_3[1024] = { L"" };
    WCHAR res_str[10];
    int itemIndex=0;
    TCHAR buff[MAX_PATH];
    LPCWSTR a = L"";
    std::wstring check;
    std::wstring uninstall_string;
    std::wstring reg_key_name;
    std::wstring display_name;
    DWORD dWord;
    DWORD dwByte;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            //Uninstall application
            case ID_BTN_UNINSTALL:
                i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                //SendMessage(hListBox, LB_GETCURSEL, i, 0);
                
                //check = std::to_wstring(i);
                MessageBox(hWnd, regApp.at(i)._DisplayName.c_str(), L"wasd", MB_OK);
                uninstall_string = regApp.at(i)._UninstallPath;
                //MessageBox(hWnd, uninstall_string.c_str(), L"wasd", MB_OK);
                _uninstall_app(uninstall_string);
                break;

            //Delete application from registry
            case ID_BTN_DELETE:
                i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                SendMessage(hListBox, LB_GETCURSEL, i, 0);
                itemIndex = i;
                check = std::to_wstring(itemIndex);
                MessageBox(hWnd, regApp.at(itemIndex)._RegKeyName.c_str(), L"wasd", MB_OK);
                reg_key_name = regApp.at(itemIndex)._RegKeyName;
                dwByte = regApp.at(itemIndex)._DwType;
                _delete_app_from_registry(reg_key_name, dwByte);
                break;
            case ID_BTN_RENAME:
                break;
            case ID_BTN_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            
            EndPaint(hWnd, &ps);
        }
        break;
    /*case WM_MINIMIZEBOX:
        TrayDrawIcon(hWnd);
        ShowWindow(hWnd, SW_HIDE);
        break;*/
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void TrayDrawIcon(HWND hWnd)
{
    NOTIFYICONDATA nid;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICONUID;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
    LoadString(hInst, IDS_APP_TITLE, nid.szTip, 128);
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    Shell_NotifyIcon(NIM_ADD, &nid);

}


void _call_64_bit(HWND& hList)
{
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
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sRoot, 0, KEY_READ | KEY_WOW64_64KEY, &hUninstKey) != ERROR_SUCCESS)
    {
        //return false;

    }

    for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
    {
        //Enumerate all sub keys...
        dwBufferSize1 = sizeof(sAppKeyName);
        if ((lResult = RegEnumKeyEx(hUninstKey, dwIndex, (LPWSTR)sAppKeyName,
            &dwBufferSize1, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            //Open the sub key.
            wsprintf(sSubKey, L"%s\\%s", sRoot, sAppKeyName);

            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_READ | KEY_WOW64_64KEY, &hAppKey) != ERROR_SUCCESS)
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
                //wprintf(L"%s\n", sDisplayName);

                //SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)sDisplayName);
            }
            else {
                //Display name value doe not exist, this application was probably uninstalled.
            }
            dwBufferSize1 = sizeof(sUninstallPath);
            if (RegQueryValueEx(hAppKey, L"UninstallString", NULL,
                &dwType, (unsigned char*)sUninstallPath, &dwBufferSize1) == ERROR_SUCCESS)
            {
                regApp.push_back(RegApplication(sUninstallPath, sDisplayName, sAppKeyName, dwByte));
            }
            else {
                //Display name value doe not exist, this application was probably uninstalled.
            }

            RegCloseKey(hAppKey);
        }
    }
    RegCloseKey(hUninstKey);
}

void _call_32_bit(HWND &hListBox)
{
    //setlocale(LC_ALL, "Russian");
    HKEY hUninstKey = NULL;
    HKEY hAppKey = NULL;
    WCHAR sAppKeyName[1024];
    WCHAR sSubKey[1024];
    WCHAR sDisplayName[1024];
    WCHAR sRoot[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    long lResult = ERROR_SUCCESS;
    DWORD dwType = KEY_READ;
    DWORD dwByte = KEY_ALL_ACCESS | KEY_WOW64_32KEY;
    DWORD dwBufferSize = 0;

    //Open the "Uninstall" key.
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sRoot, 0, KEY_READ | KEY_WOW64_32KEY, &hUninstKey) != ERROR_SUCCESS)
    {
        //return false;
        /*std::cout << "Fail" << std::endl;*/
    }

    for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
    {
        //Enumerate all sub keys...
        dwBufferSize = sizeof(sAppKeyName);
        if ((lResult = RegEnumKeyEx(hUninstKey, dwIndex, sAppKeyName,
            &dwBufferSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            //Open the sub key.
            wsprintf(sSubKey, L"%s\\%s", sRoot, sAppKeyName);
            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_READ | KEY_WOW64_32KEY, &hAppKey) != ERROR_SUCCESS)
            {
                RegCloseKey(hAppKey);
                RegCloseKey(hUninstKey);
                //return false;
            }

            //Get the display name value from the application's sub key.
            dwBufferSize = sizeof(sDisplayName);
            if (RegQueryValueEx(hAppKey, L"DisplayName", NULL,
                &dwType, (unsigned char*)sDisplayName, &dwBufferSize) == ERROR_SUCCESS)
            {
                //wprintf(L"%s\n", sDisplayName);

                //SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)sDisplayName);
            }
            else {
                //Display name value doe not exist, this application was probably uninstalled.
            }
            dwBufferSize = sizeof(sUninstallPath);
            if (RegQueryValueEx(hAppKey, L"UninstallString", NULL,
                &dwType, (unsigned char*)sUninstallPath, &dwBufferSize) == ERROR_SUCCESS)
            {

                regApp.push_back(RegApplication(sUninstallPath, sDisplayName, sAppKeyName, dwByte));

            }
            else {
                //Display name value doe not exist, this application was probably uninstalled.
            }

            RegCloseKey(hAppKey);
        }
    }
   
    RegCloseKey(hUninstKey);
}



void _delete_app_from_registry(std::wstring& reg_key_name, DWORD& dWord)
{
    HKEY hKey;
    
    std::wstring path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" +  reg_key_name;
    
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, dWord, &hKey) == ERROR_SUCCESS)
    {
        if (RegDeleteKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), dWord, NULL) == ERROR_SUCCESS)
        {
            MessageBox(NULL, L"Программа удалена из реестра", L"Сообщение", MB_OK);
        }
        else {
            MessageBox(NULL, L"Произошла ошибка!", L"Сообщение", MB_OK);
        }
    }
}

void _uninstall_app(std::wstring& uninstal_string)
{
    int a = 5;
    const wchar_t* input = uninstal_string.c_str();
    size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
    char* buffer = new char[size];
    size_t convertedSize;
    wcstombs_s(&convertedSize, buffer, size, input, size);
    system(buffer);
    //ShellExecute(NULL, L"open", L"msiexec", L" /I{FFB40224-64C0-4D82-ADC4-6B9434B90800}", NULL, SW_MINIMIZE);
    //ShellExecute(NULL, L"open", uninstal_string.c_str(), NULL, NULL, SW_SHOW);

}


bool compareByLength(const RegApplication& a, const RegApplication& b)
{
    return a._DisplayName < b._DisplayName;
}

void _output_vector(HWND& hListBox)
{
    //std::sort(regApp.begin(), regApp.end(), compareByLength);
    sort(regApp.begin(), regApp.end(), RegAppCmp());
    //for (int i = 0; i <= regApp.size(); i++)
    //{
    //    SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)regApp[i]._DisplayName.c_str());
    //    //SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)regApp[i]._RegKeyName.c_str());
    //    //SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)regApp[i]._UninstallPath.c_str());
    //}
    for (auto const& element : regApp)
    {
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)element._DisplayName.c_str());
    }
}
int iLine;             // Receives line number.
BOOL fRelative;        // Receives check box status. 

BOOL CALLBACK GoToProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL fError;

    switch (message)
    {
    case WM_INITDIALOG:
        CheckDlgButton(hwndDlg, ID_ABSREL, fRelative);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            fRelative = IsDlgButtonChecked(hwndDlg, ID_ABSREL);
            iLine = GetDlgItemInt(hwndDlg, IDС_EDIT1, &fError, fRelative);
            if (fError)
            {
                MessageBox(hwndDlg, L"", L"", MB_OK);
                SendDlgItemMessage(hwndDlg, IDС_EDIT1, EM_SETSEL, 0, -1L);
            }
            else

                // Notify the owner window to carry out the task. 

                return TRUE;

        case IDCANCEL:
            DestroyWindow(hwndDlg);
            hwndGoto = NULL;
            return TRUE;
        }
    }
    return FALSE;
}