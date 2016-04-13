#pragma once
#include "Object.h"
#include "stdinclude.h"
#include "PlayerPacket.h"

extern map<SkinID, pair<VBOID, vtxCount>> playerSkins;

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