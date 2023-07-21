#include "internal.h"

// Reference: https://github.com/metaxor/KernelEx/blob/31cdfc3560fc116637ee8ed7be31b12f3aacf5d1/common/common.h#L143

#define STACK_AtoW(strA,strW) \
    { \
        strW = (LPWSTR)strA; \
        if (HIWORD(strA)) \
        { \
            int c = lstrlenA((LPCSTR)strA); \
            if (c) \
            { \
                strW = (LPWSTR)alloca(c * sizeof(WCHAR)); \
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strA, -1, (LPWSTR)strW, c); \
            } \
        } \
    }

// GetMonitorInfoW polyfill
// https://learn.microsoft.com/ru-ru/windows/win32/api/winuser/nf-winuser-getmonitorinfow
// Reference: https://github.com/metaxor/KernelEx/blob/31cdfc3560fc116637ee8ed7be31b12f3aacf5d1/apilibs/kexbasen/user32/uniuser32.c#L404
//

BOOL GLFW_GetMonitorInfoW(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
    // TODO: return the actual screen size
    return FALSE;
}

// SetThreadExecutionState polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadexecutionstate
//

EXECUTION_STATE GLFW_SetThreadExecutionState(EXECUTION_STATE esFlags)
{
    // TODO: figure out how to prevent the screen from turning off on Windows 2000
    return NULL;
}

// GetModuleHandleExW polyfill
// https://learn.microsoft.com/ru-ru/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandleexw
// Reference: https://github.com/metaxor/KernelEx/blob/31cdfc3560fc116637ee8ed7be31b12f3aacf5d1/apilibs/kexbases/Kernel32/module.c#L108
//

BOOL GLFW_GetModuleHandleExW(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule)
{
    WCHAR buf[MAX_PATH];
    if (!phModule)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    *phModule = NULL;
    if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(lpModuleName, &mbi, sizeof(mbi)))
            return FALSE;

        *phModule = (HMODULE) mbi.AllocationBase;
    }
    else
        *phModule = GetModuleHandleW(lpModuleName);

    if (*phModule == NULL || !GetModuleFileNameW(*phModule, buf, MAX_PATH))
    {
        *phModule = NULL;
        return FALSE;
    }

    if (!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT))
        LoadLibraryW(buf);

    return TRUE;
}

// VerSetConditionMask polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-versetconditionmask
// Reference: https://github.com/metaxor/KernelEx/blob/31cdfc3560fc116637ee8ed7be31b12f3aacf5d1/apilibs/kexbases/Kernel32/version.c#L383
//

inline ULONGLONG GLFW_VerSetConditionMask(ULONGLONG dwlConditionMask, DWORD dwTypeBitMask, BYTE dwConditionMask)
{
    if (dwTypeBitMask == 0)
        return dwlConditionMask;

    dwConditionMask &= 0x07;
    if (dwConditionMask == 0)
        return dwlConditionMask;

    if (dwTypeBitMask & VER_PRODUCT_TYPE)
        dwlConditionMask |= dwConditionMask << 7*3;
    else if (dwTypeBitMask & VER_SUITENAME)
        dwlConditionMask |= dwConditionMask << 6*3;
    else if (dwTypeBitMask & VER_SERVICEPACKMAJOR)
        dwlConditionMask |= dwConditionMask << 5*3;
    else if (dwTypeBitMask & VER_SERVICEPACKMINOR)
        dwlConditionMask |= dwConditionMask << 4*3;
    else if (dwTypeBitMask & VER_PLATFORMID)
        dwlConditionMask |= dwConditionMask << 3*3;
    else if (dwTypeBitMask & VER_BUILDNUMBER)
        dwlConditionMask |= dwConditionMask << 2*3;
    else if (dwTypeBitMask & VER_MAJORVERSION)
        dwlConditionMask |= dwConditionMask << 1*3;
    else if (dwTypeBitMask & VER_MINORVERSION)
        dwlConditionMask |= dwConditionMask << 0*3;
    return dwlConditionMask;
}

// EnumDisplayMonitors polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumdisplaymonitors
//

BOOL GLFW_EnumDisplayMonitors(HDC hdc, LPCRECT lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData)
{
    // TODO: call the callback at least once for one monitor
    return FALSE;
}

// EnumDisplaySettingsExW polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumdisplaysettingsexw
//

BOOL GLFW_EnumDisplaySettingsExW(LPCWSTR lpszDeviceName, DWORD iModeNum, DEVMODEW *lpDevMode, DWORD dwFlags)
{
    // TODO:
    return FALSE;
}

// EnumDisplaySettingsW polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumdisplaysettingsw
//

BOOL GLFW_EnumDisplaySettingsW(LPCWSTR  lpszDeviceName, DWORD iModeNum, DEVMODEW *lpDevMode)
{
    // TODO:
    return FALSE;
}

// GetLayeredWindowAttributes polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getlayeredwindowattributes
//

BOOL GLFW_GetLayeredWindowAttributes(HWND hwnd, COLORREF *pcrKey, BYTE *pbAlpha, DWORD *pdwFlags)
{
    // TODO:
    return FALSE;
}

// SetLayeredWindowAttributes polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setlayeredwindowattributes
//

BOOL GLFW_SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
    // TODO:
    return FALSE;
}

// GetRawInputData polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputdata
//

UINT GLFW_GetRawInputData(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader)
{
    // TODO:
    return -1;
}

// GetRawInputDeviceInfoA polyfill
// https://learn.microsoft.com/ru-ru/windows/win32/api/winuser/nf-winuser-getrawinputdeviceinfoa
//

UINT GLFW_GetRawInputDeviceInfoA(HANDLE hDevice, UINT uiCommand, LPVOID pData, PUINT pcbSize)
{
    // TODO:
    return -1;
}

// GetRawInputDeviceList polyfill
// https://learn.microsoft.com/ru-ru/windows/win32/api/winuser/nf-winuser-getrawinputdevicelist
//

UINT GLFW_GetRawInputDeviceList(PRAWINPUTDEVICELIST pRawInputDeviceList, PUINT puiNumDevices, UINT cbSize)
{
    // TODO:
    return -1;
}

// MonitorFromWindow polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-monitorfromwindow
//

HMONITOR GLFW_MonitorFromWindow(HWND hwnd, DWORD dwFlags)
{
    // TODO:
    return NULL;
}

// RegisterDeviceNotificationW polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerdevicenotificationw
//

HDEVNOTIFY GLFW_RegisterDeviceNotificationW(HANDLE hRecipient, LPVOID NotificationFilter, DWORD Flags)
{
    // TODO:
    return NULL;
}

// RegisterRawInputDevices polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices
//

BOOL GLFW_RegisterRawInputDevices(PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize)
{
    return FALSE;
}

// UnregisterDeviceNotification polyfill
// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unregisterdevicenotification
//

BOOL GLFW_UnregisterDeviceNotification(HDEVNOTIFY Handle)
{
    return FALSE;
}