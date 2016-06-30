/'
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
'/

#include "../api/freebasic/nwapi.bi"

' Convert const string to zstring ptr
function c_str(byref s as const string) as zstring ptr
    dim res as zstring ptr = new byte[len(s) + 1]
    *res = s
    return res
end function

extern "C"
    ' Main function
    function cdecl init() as PluginData ptr
        dim testPlugin as PluginData ptr = new PluginData
        testPlugin->pluginName = c_str("Test Plugin")
        testPlugin->authorName = c_str("INFINIDEAS")
        testPlugin->internalName = c_str("infinideas.testplugin")
        return testPlugin
    end function
end extern
