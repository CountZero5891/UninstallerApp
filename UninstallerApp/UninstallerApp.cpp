
// UninstallerApp.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "UninstallerApp.h"
#include "regappfunc.h"
#include <ShlObj.h>

#define MAX_LOADSTRING 100
#define BUFFER 8192

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hListBox, hBtn1, hBtn2, hBtn3, hBtn4;
HWND hwndGoto = NULL;
HWND editText = NULL;
BOOL isAdmin;

//Глобальная переменная, вектор структур 
std::vector<RegApplication> regApp;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK    GoToProc(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK EditAppNameForm(HWND, UINT, WPARAM, LPARAM);

void TrayDrawIcon(HWND hWnd);//System Tray
void TrayDeleteIcon(HWND hWnd);//Delete tray icon


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

    //Dialog Box
    //DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, (DlgProc), 0);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UNINSTALLERAPP));

    MSG msg;
    BOOL bRet;
    // Цикл основного сообщения:
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}
    
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

    //CloseHandle(Test_Present);
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
    wcex.lpszMenuName   = NULL;
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
       WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,
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
   isAdmin = IsUserAnAdmin();
   _call_user_apps(hListBox);
   _call_64_bit(hListBox);
   _call_32_bit(hListBox);
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
    int msgBx = 0;
    std::wstring check;
    std::wstring uninstall_string;
    std::wstring reg_key_name;
    std::wstring display_name;
    DWORD dwByte;
    HKEY startHKey = NULL;
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

                    msgBx = MessageBox(hWnd, L"Вы хотите удалить это приложение", L"Предупреждение", MB_ICONEXCLAMATION | MB_YESNO);
                    if (msgBx == IDYES)
                    {
                        i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                        SendMessage(hListBox, LB_GETCURSEL, i, 0);
                        uninstall_string = regApp.at(i)._UninstallPath;
                        _uninstall_application(uninstall_string);
                    }

                break;
            //Delete application from registry
            case ID_BTN_DELETE:

                if (isAdmin == TRUE)
                {
                    i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                    
                    SendMessage(hListBox, LB_GETCURSEL, i, 0);
                    reg_key_name = regApp.at(i)._RegKeyName;
                    dwByte = regApp.at(i)._DwType;
                    startHKey = regApp.at(i)._RegAppHKEY;
                    _delete_app_from_registry(reg_key_name, dwByte, startHKey);
                    regApp.erase(next(regApp.begin()));
                    SendMessage(hListBox, LB_DELETESTRING, i, 0);
                }
                else {
                    MessageBox(hWnd, L"Данная операция разрешена только администратору", L"Ошибка", MB_OK);
                }

                
                break;
            //rename application in registry
            case ID_BTN_RENAME:
                if (isAdmin == TRUE)
                {
                    DialogBox(hInst,                   // application instance
                        MAKEINTRESOURCE(IDD_DIALOG1), // dialog box resource
                        hWnd,                          // owner window
                        EditAppNameForm                    // dialog box window procedure
                    );
                }
                else {
                    MessageBox(hWnd, L"Данная операция разрешена только админиcтратору", L"Ошибка", MB_OK);
                }
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
            SendMessage(hListBox, LB_SETCURSEL, 0, 0);
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SYSCOMMAND:
        switch (wParam)
        {
        case SC_MINIMIZE:
            TrayDrawIcon(hWnd);
            ShowWindow(hWnd, SW_HIDE);
            break;
        case SC_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_TRAYMESSAGE:
        switch (lParam) {
        case WM_LBUTTONDBLCLK:
            ShowWindow(hWnd, SW_SHOW);
            TrayDeleteIcon(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
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

INT_PTR CALLBACK EditAppNameForm(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WCHAR res[2048];
    std::wstring set_display_name;
    std::wstring reg_key_name;
    HKEY startHKey = NULL;
    int i = 0;
    switch (message)
    {
    case WM_INITDIALOG:
        i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
        SendMessage(hListBox, LB_GETCURSEL, i, 0);
        editText = GetDlgItem(hDlg, IDC_EDIT1);
        SendMessage(editText, WM_SETTEXT, 0, (LPARAM)regApp.at(i)._DisplayName.c_str());
        reg_key_name = regApp.at(i)._RegKeyName;
        break;

    case WM_COMMAND:
        // Set the default push button to "OK" when the user enters text. 
        switch (wParam)
        {
        case IDOK:
            i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
            GetWindowText(editText, res, 2048);
            set_display_name = std::wstring(res);
            reg_key_name = regApp.at(i)._RegKeyName;
            startHKey = regApp.at(i)._RegAppHKEY;
            _rename_app_in_registry(set_display_name, reg_key_name, regApp.at(i)._DwType, startHKey, i);
            SendMessage(hListBox, LB_DELETESTRING, i, 0);
            SendMessage(hListBox, LB_INSERTSTRING, i, (LPARAM)res);
            EndDialog(hDlg, LOWORD(wParam));
            break;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            break;
        }
        return 0;
    }
    return FALSE;

    UNREFERENCED_PARAMETER(lParam);
}

void TrayDrawIcon(HWND hWnd)
{
    NOTIFYICONDATA nid;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICONUID;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.uCallbackMessage = WM_TRAYMESSAGE;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
    LoadString(hInst, IDS_APP_TITLE, nid.szTip, 128);
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    Shell_NotifyIcon(NIM_ADD, &nid);

}

void TrayDeleteIcon(HWND hWnd)
{
    NOTIFYICONDATA nid;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICONUID;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}


/*
* Ты добавил проверку роли пользователя
* Исправил функцию переименования
* Осталось исправить удаление и поиск программ
* Сделай
*/
