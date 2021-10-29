#ifndef REGAPPFUNC_H
#define REGAPPFUNC_H

#include "framework.h"
#include <string>
#include <string>
#include <strsafe.h>
#include <vector>
#include <algorithm>
#include "ShellAPI.h"

struct RegApplication
{
    std::wstring _UninstallPath;
    std::wstring _DisplayName;
    std::wstring _RegKeyName;
    HKEY _RegAppHKEY;
    DWORD _DwType;
    
    RegApplication(WCHAR unPath[1023], WCHAR dispName[1023], WCHAR regKeyName[1023], HKEY hKeyReg, DWORD dwType)
    {
        _UninstallPath = unPath;
        _DisplayName = dispName;
        _RegKeyName = regKeyName;
        _RegAppHKEY = hKeyReg;
        _DwType = dwType;
    };
};

void _call_user_apps(HWND& hList);//Read from user directory

void _call_64_bit(HWND& hList);//Read for 64 bit

void _call_32_bit(HWND& hList);//read for 32 bit

void _output_vector(HWND& hListBox);//output sorted vector to ListBox

void _uninstall_application(std::wstring uninstall_string);//uninstall apllication function

void _uninstall_app(std::wstring& uninstal_string);//deprecated

void _delete_app_from_registry(std::wstring& reg_key_name, DWORD& dWord, HKEY StartKey);//delete app from registry function

void _rename_app_in_registry(std::wstring& set_value, std::wstring reg_key_name, DWORD dwByte, HKEY StartKey); //rename application in registry function


#endif