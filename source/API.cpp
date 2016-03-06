#include "API.h"
#include "Player.h"

std::map<std::string, void*> Mod::sharedData;

APIPackage Mod::getPackage()
{
    static APIPackage api;
    static bool init = false;
    if (init) return api;
    api.getChunk = World::getChunkPtr;
    api.getBlock = std::bind(World::getblock, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 0, nullptr);
    api.setBlock = std::bind(World::setblock, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, nullptr);
    api.getCommand = [](string s) -> Command*
    {
        for (size_t i = 0; i < commands.size(); i++)
            if (commands[i].identifier == s) return &commands[i];
        return nullptr;
    };
    api.registerCommand = [](Command c) -> bool
    {
        for (size_t i = 0; i < commands.size(); i++)
            if (commands[i].identifier == c.identifier) return false;
        commands.push_back(c);
        return true;
    };
    api.getSharedData = [](std::string key) -> void*
    {
        std::map<std::string, void*>::iterator iter = sharedData.find(key);
        if (iter == sharedData.end()) return nullptr;
        return iter->second;
    };
    api.setSharedData = [](std::string key, void* value)
    {
        sharedData[key] = value;
    };
    api.getPlayerData = []()->PlayerData
    {
        PlayerData player;
        player.AirJumps = Player::AirJumps;
        player.BlockInHand = Player::BlockInHand;
        player.CrossWall = Player::CrossWall;
        player.cxt = Player::cxt;
        player.cxtl = Player::cxtl;
        player.cyt = Player::cyt;
        player.cytl = Player::cytl;
        player.czt = Player::czt;
        player.cztl = Player::cztl;
        player.dropDamage = Player::dropDamage;
        player.Flying = Player::Flying;
        player.Glide = Player::Glide;
        player.glidingEnergy = Player::glidingEnergy;
        player.glidingMinimumSpeed = Player::glidingMinimumSpeed;
        player.glidingNow = Player::glidingNow;
        player.glidingSpeed = Player::glidingSpeed;
        player.heading = Player::heading;
        player.healSpeed = Player::healSpeed;
        player.health = Player::health;
        player.healthMax = Player::healthMax;
        player.height = Player::height;
        player.heightExt = Player::heightExt;
        //player.Hitboxes = Player::Hitboxes;
        player.indexInHand = Player::indexInHand;
        player.inventory = (item*)Player::inventory;
        player.inventoryAmount = (short*)Player::inventoryAmount;
        player.inWater = Player::inWater;
        player.jump= Player::jump;
        player.lookupdown = Player::lookupdown;
        //player.name = Player::name;
        player.NearWall = Player::NearWall;
        player.OnGround = Player::OnGround;
        player.onlineID = Player::onlineID;
        player.playerbox = Player::playerbox;
        player.Running = Player::Running;
        player.speed = Player::speed;
        player.ViewFrustum = Player::ViewFrustum;
        player.xa = Player::xa;
        player.xd = Player::xd;
        player.xlookspeed = Player::xlookspeed;
        player.xpos = Player::xpos;
        player.xposold = Player::xposold;
        player.ya = Player::ya;
        player.yd = Player::yd;
        player.ylookspeed = Player::ylookspeed;
        player.ypos = Player::ypos;
        player.yposold = Player::yposold;
        player.za = Player::za;
        player.zd = Player::zd;
        player.zpos = Player::zpos;
        player.zposold = Player::zposold;
        return player;
    };
    api.updateBlock = std::bind(World::updateblock, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, 0);
    api.setChunkUpdated = World::setChunkUpdated;
    init = true;
    return api;
}
