/*
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
*/

using System;
using System.Runtime.InteropServices;

namespace ManagedPluginAPI
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vec3i
    {
        int x, y, z;
    }
    [StructLayout(LayoutKind.Explicit)]
    public struct BlockData
    {
        [FieldOffset(12)]
        uint id;
        [FieldOffset(4)]
        uint brightness;
        [FieldOffset(16)]
        uint state;
    }
    [StructLayout(LayoutKind.Sequential)]
    public struct BlockType
    {
        [MarshalAs(UnmanagedType.LPStr)]
        string blockname;
        [MarshalAs(UnmanagedType.I1)]
        bool solid;
        [MarshalAs(UnmanagedType.I1)]
        bool translucent;
        [MarshalAs(UnmanagedType.I1)]
        bool opaque;
        int explodePower;
        int hardness;
    }
    public class PluginAPI
    {
        public static BlockData getBlock(Vec3i pos)
        {
            return _getBlock(pos);
        }
        public static void setBlock(Vec3i pos, BlockData data)
        {
            _setBlock(pos, data);
        }
        public static void registerBlock(BlockType type)
        {
            _registerBlock(type);
        }
        [DllImport("PluginAPI", EntryPoint = "getBlock")]
        private static extern BlockData _getBlock([MarshalAs(UnmanagedType.LPStruct)]Vec3i pos);
        [DllImport("PluginAPI", EntryPoint = "setBlock")]
        private static extern void _setBlock([MarshalAs(UnmanagedType.LPStruct)]Vec3i pos, BlockData data);
        [DllImport("PluginAPI", EntryPoint = "registerBlock")]
        private static extern void _registerBlock([MarshalAs(UnmanagedType.LPStruct)]BlockType type);
    }
}
