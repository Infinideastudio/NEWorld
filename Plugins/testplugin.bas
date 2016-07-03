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

' Plugin data
dim shared TestPlugin as PluginData ptr

' Convert const string to zstring ptr
function c_str(byref s as const string) as zstring ptr
    dim res as zstring ptr = new byte[len(s) + 1]
    *res = s
    return res
end function

extern "C"
    ' Main function
    function init cdecl() as PluginData ptr export
        dim rock as BlockType
        with rock
            .blockname = c_str("Rock")
            .solid = 1
            .translucent = 0
            .opaque = 1
            .explodePower = 0
            .hardness = 2
        end with
        registerBlock(@rock)
        TestPlugin = new PluginData
        TestPlugin->pluginName = c_str("Test Plugin")
        TestPlugin->authorName = c_str("INFINIDEAS")
        TestPlugin->internalName = c_str("infinideas.testplugin")
        return TestPlugin
    end function
end extern
