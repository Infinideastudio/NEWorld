#ifndef EXPORT_VARIABLES_H
#define EXPORT_VARIABLES_H

#include "pluginsdk.h"
#include <vector>
#include <string>
#include <map>
struct command
{
	std::wstring identifier;
	command_function function;

	command()
	{
	}

	command(const std::wstring& _i, command_function _func)
		:identifier(_i), function(_func)
	{
	}

	int execute(const std::vector<std::wstring>& args)
	{
		wchar_t const** argv = new wchar_t const*[args.size()];
		for (size_t i = 0; i < args.size(); i++)
			argv[i] = args[i].c_str();
		int ret = function((int)args.size(), argv);
		delete[] argv;
		return ret;
	}
};

typedef unsigned short(*get_block_func)(int x, int y, int z);
typedef void(*update_block_func)(int x, int y, int z, bool val);

extern get_block_func get_func;
extern update_block_func update_func;
DLLIMPEXP extern std::map<std::wstring, command> commands;
extern "C" DLLIMPEXP void init(get_block_func _get, update_block_func _update);
#endif
