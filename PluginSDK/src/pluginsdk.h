#ifndef EXPORT_FUNCS_H
#define EXPORT_FUNCS_H

#ifdef NEWORLD_TARGET_WINDOWS

#ifdef PLUGINSDK_EXPORTS
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    wchar_t *name, *description;
} plugin_information;

typedef plugin_information* (*get_information)();
typedef int(*init_function)();
typedef void(*unload_function)();
typedef int(*command_function)(int argc, const wchar_t** argv);

DLLIMPEXP void load_plugins(wchar_t const * const filename);
DLLIMPEXP void unload_plugins();

DLLIMPEXP void* get_shared_data(wchar_t const * const key);
DLLIMPEXP void set_shared_data(wchar_t const * const key, void* const data);

DLLIMPEXP command_function get_command(wchar_t const * const name);
DLLIMPEXP void register_command(wchar_t const * const name, command_function func);

DLLIMPEXP unsigned short get_block(int x, int y, int z);
DLLIMPEXP void update_block(int x, int y, int z, char val);
#ifdef __cplusplus
}
#endif

#endif //NEWORLD_TARGET_WINDOWS

#endif //EXPORT_FUNCS_H
