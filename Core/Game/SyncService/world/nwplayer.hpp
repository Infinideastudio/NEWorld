// 
// Core: nwplayer.hpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#ifndef PLAYER_MANAGER_H_
#define PLAYER_MANAGER_H_

#include <Common/common.h>
#include <unordered_map>
#include <string>

using PlayerID = std::string; // Player ID is player's name

struct PlayerData {
    PlayerID mUid;
    bool mOnline;
    std::string mKey;

    Vec3d mSpeed, mDirection, mPosition, mRotation, mScale;
    AABB mHitbox;
};

class PlayerManager {
public:
    PlayerData* getPlayer(const PlayerID& uid) {
        auto player = mPlayers.find(uid);
        if (player == mPlayers.end()) {
            warningstream << "Failed to find player:" << uid;
            return nullptr;
        }
        return &player->second;
    }

    PlayerData* loginOrRegister(const PlayerID& uid, const std::string& key) {
        auto& player = mPlayers[uid];
        if (player.mUid.empty()) // new user
        {
            player.mUid = uid;
            // init player data
        }
        else {
            if (player.mKey != key) //wrong password
                return nullptr;
        }

        player.mOnline = true;
        return &player;
    }

private:
    std::unordered_map<PlayerID, PlayerData> mPlayers;
};

#endif // !PLAYER_MANAGER_H_
