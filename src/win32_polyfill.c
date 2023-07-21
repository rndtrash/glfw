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
    LPWSTR lpDevice = NULL;
    BOOL result;
    MONITORINFOEX miex;

    if (IsBadWritePtr(lpmi, sizeof(MONITORINFO)))
        return FALSE;

    if (lpmi->cbSize == sizeof(MONITORINFO))
        return GetMonitorInfoA(hMonitor, lpmi);

    if (lpmi->cbSize != sizeof(MONITORINFOEXW))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    miex.cbSize = sizeof(MONITORINFOEXA);

    result = GetMonitorInfoA(hMonitor, &miex);

    if(!result)
        return FALSE;

    STACK_AtoW(miex.szDevice, lpDevice);

    return result;
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