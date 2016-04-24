#ifndef DEFS_H
#define DEFS_H

#ifdef _WIN32
#include <Windows.h>

typedef HMODULE dll_handle;

inline dll_handle load_library(wchar_t const * const file_name)
{
    return LoadLibraryW(file_name);
}

inline void* get_address(dll_handle handle, char const * const symbol_name)
{
    return GetProcAddress(handle, symbol_name);
}

inline void unload_library(dll_handle handle)
{
    FreeLibrary(handle);
}

#elif defined(__unix__) || defined(unix)
#include <dlfcn.h>

typedef void* dll_handle;

inline dll_handle load_library(wchar_t const * const file_name)
{
    size_t len = wcslen(file_name);
    unique_ptr<char> tmp(new char[len * 3]);
    wcstombs(tmp.get(), file_name, len);
    return dlopen(tmp.get(), RTLD_DEFAULT);
}

inline void* get_address(dll_handle handle, char const * const symbol_name)
{
    size_t len = wcslen(symbol_name);
    unique_ptr<char> tmp(new char[len * 3]);
    wcstombs(tmp.get(), symbol_name, len);
    return dlsym(handle, tmp.get());
}

inline void unload_library(dll_handle handle)
{
    dlclose(handle);
}

#endif

#include <string>
struct internal_plugin_info
{
    dll_handle handle;
    std::wstring name, description;
    internal_plugin_info()
    {
    }
    internal_plugin_info(dll_handle _handle, const std::wstring& _name, const std::wstring& _desc)
        :handle(_handle), name(_name), description(_desc)
    {
    }
};
#endif
