/*
 * NEWorld: An free game with similar rules to Minecraft.
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

#ifndef ONLINEPLAYER_H
#define ONLINEPLAYER_H

#include "Definitions.h"
#include "Object.h"
#include "PlayerPacket.h"

extern map<SkinID, pair<VBOID, vtxCount> > playerSkins;

class OnlinePlayer : public Object
{
    public:
        OnlinePlayer(double x, double y, double z, string name, onlineid onlineID, SkinID skinID, double lookupdown, double heading) :
            Object(x, y, z), _name(name), _onlineID(onlineID), _skinID(skinID), _lookupdown(lookupdown), _heading(heading) {}

        OnlinePlayer(PlayerPacket& p) :
            OnlinePlayer(p.x, p.y, p.z, p.name, p.onlineID, p.skinID, p.lookupdown, p.heading) {}

        const string& getName() const
        {
            return _name;
        }

        const onlineid getOnlineID() const
        {
            return _onlineID;
        }

        void GenVAOVBO(int skinID);

        void buildRenderIfNeed();

        void render() const;

    private:
        string _name;
        onlineid _onlineID;
        SkinID _skinID;
        double _lookupdown, _heading;
};

extern vector<OnlinePlayer> players;

#endif
