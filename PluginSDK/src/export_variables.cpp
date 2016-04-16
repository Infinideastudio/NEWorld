#include "export_variables.h"

std::map<std::wstring, command> commands;
get_block_func get_func;
update_block_func update_func;

DLLIMPEXP void init(get_block_func _get, update_block_func _update)
{
    get_func = _get;
    update_func = _update;
}
