
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

//class AppInfo {
//    WCHAR displayName[1024];
//    WCHAR sUninstallString[1024];
//    WCHAR keyName[1024];
//
//
//};
WCHAR sUninstallPath[1024];
WCHAR sDisplayName[1024];
std::vector<std::wstring> dsNm;


struct RegApplication {
    
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

} ;

std::vector<RegApplication> regApp;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void _call_64_bit(HWND& hList);
void _call_32_bit(HWND& hList);
void _output_vector(HWND& hListBox);
bool compareByLength(const RegApplication& a, const RegApplication& b);
void _uninstall_app();
void _find_uninstall_string();

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

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
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

   

   if (!hWnd)
   {
      return FALSE;
   }
   _call_64_bit(hListBox);
   _call_32_bit(hListBox);
   //sort(regApp.begin(), regApp.end(), compareByLength);
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
    switch (message)
    {
    
    //case WM_INITDIALOG:
    //{
    //    for (int i = 0; i < regApp.size(); i++)
    //    {
    //        int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0,
    //            (LPARAM)regApp[i]._DisplayName.c_str());
    //        // Set the array index of the player as item data.
    //        // This enables us to retrieve the item from the array
    //        // even after the items are sorted by the list box.
    //        SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)i);
    //    }
    //    // Set input focus to the list box.
    //    SetFocus(hwndList);
    //    return TRUE;
    //}

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case ID_BTN_UNINSTALL:
                i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                SendMessage(hListBox, LB_GETCURSEL, i, 0);
                itemIndex = i;
                check = std::to_wstring(itemIndex);
                MessageBox(hWnd, regApp.at(itemIndex)._DisplayName.c_str(), L"wasd", MB_OK);
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
                //regApp.at(dwIndex).push_back();
                regApp.push_back(RegApplication(sUninstallPath, sDisplayName, sAppKeyName, dwType));
                //StringCbPrintfA();
                //wprintf(L"%s\n", sDisplayName);
                //SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)sUninstallPath);
            }
            else {
                //Display name value doe not exist, this application was probably uninstalled.
            }


            RegCloseKey(hAppKey);
        }
    }
    /*for (int i = 0; i < regApp.size(); i++)
    {
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)regApp[i]._DisplayName.c_str());
    }*/
    RegCloseKey(hUninstKey);
}

void _uninstall_app()
{
    HKEY hKey = NULL;
    /*TCHAR dwBuffer[1024];*/
    wchar_t value[512];
    WCHAR srt[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
    DWORD BufferSize = BUFFER;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" ), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        //std::cout << "Success opening key." << std::endl;
        ULONG bRes = RegGetValueW(HKEY_LOCAL_MACHINE,
            TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"),
            TEXT("UninstallString"),
            RRF_RT_ANY,
            NULL,
            (PVOID)&value,
            &BufferSize);
        if (bRes == ERROR_SUCCESS)
        {
            //_tprintf(TEXT("(%d) %s - %s\n"), hKey, value);
            ShellExecute(NULL, L"open", value, NULL, NULL, SW_SHOW);
            //std::cout<<*value<<std::endl;
        }

    }
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
                //regApp.at(dwIndex).push_back();
                regApp.push_back(RegApplication(sUninstallPath, sDisplayName, sAppKeyName, dwType));
                //StringCbPrintfA();
                //wprintf(L"%s\n", sDisplayName);
                //SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)sUninstallPath);
            }
            else {
                //Display name value doe not exist, this application was probably uninstalled.
            }

            RegCloseKey(hAppKey);
        }
    }
    /*for (int i = 0; i < regApp.size(); i++)
    {
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)regApp[i]._DisplayName.c_str());
    }*/
    RegCloseKey(hUninstKey);
}

bool compareByLength(const RegApplication& a, const RegApplication& b)
{
    return a._DisplayName < b._DisplayName;
}


void _output_vector(HWND& hListBox)
{
    //std::sort(regApp.begin(), regApp.end(), compareByLength);
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