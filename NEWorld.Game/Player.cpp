#include "Player.h"
#include "Universe/World/World.h"
#include "OnlinePlayer.h"
#include <sstream>
#include <fstream>

int Player::gamemode = GameMode::Survival;
bool Player::Glide;
bool Player::Flying;
bool Player::CrossWall;
double Player::glidingMinimumSpeed = pow(1, 2) / 2;

float Player::height = 1.2f;
float Player::heightExt = 0.0f;
bool Player::OnGround = false;
bool Player::Running = false;
bool Player::NearWall = false;
bool Player::inWater = false;
bool Player::glidingNow = false;
item Player::BlockInHand = Blocks::ENV;
ubyte Player::indexInHand = 0;

Hitbox::AABB Player::playerbox;
std::vector<Hitbox::AABB> Player::Hitboxes;

double Player::xa, Player::ya, Player::za, Player::xd, Player::yd, Player::zd;
double Player::health = 20, Player::healthMax = 20, Player::healSpeed = 0.01, Player::dropDamage = 5.0;
onlineid Player::onlineID;
std::string Player::name;
Frustum Player::ViewFrustum;

double Player::speed;
int Player::AirJumps;
int Player::cxt, Player::cyt, Player::czt, Player::cxtl, Player::cytl, Player::cztl;
Double3 Player::Pos, Player::PosOld;
double Player::lookupdown, Player::heading;
double Player::jump;
double Player::xlookspeed, Player::ylookspeed;
Int3 Player::IntPos, Player::IntPosOld;

item Player::inventory[4][10];
short Player::inventoryAmount[4][10];

double Player::glidingEnergy, Player::glidingSpeed;

void InitHitbox(Hitbox::AABB &playerbox) {
    playerbox.xmin = -0.3;
    playerbox.xmax = 0.3;
    playerbox.ymin = -0.85;
    playerbox.ymax = 0.85;
    playerbox.zmin = -0.3;
    playerbox.zmax = 0.3;
}

void InitPosition() {
    Player::PosOld = Player::Pos;
    Player::cxt = World::GetChunkPos(static_cast<int>(Player::Pos.X));
    Player::cxtl = Player::cxt;
    Player::cyt = World::GetChunkPos(static_cast<int>(Player::Pos.Y));
    Player::cytl = Player::cyt;
    Player::czt = World::GetChunkPos(static_cast<int>(Player::Pos.Z));
    Player::cztl = Player::czt;
}

void MoveHitbox(double x, double y, double z) {
    Hitbox::MoveTo(Player::playerbox, x, y + 0.5, z);
}

void updateHitbox() {
    MoveHitbox(Player::Pos.X, Player::Pos.Y, Player::Pos.Z);
}

void Player::init(Double3 pos) {
	Pos = pos;
    InitHitbox(Player::playerbox);
    InitPosition();
    updateHitbox();
}

void Player::spawn() {
	Pos = Double3(0.0,60.0,0.0);
    jump = 0.0;
    InitHitbox(Player::playerbox);
    InitPosition();
    updateHitbox();
    health = healthMax;
    memset(inventory, 0, sizeof(inventory));
    memset(inventoryAmount, 0, sizeof(inventoryAmount));

    //�ܵüӵ���Ʒ��
    for (size_t i = 0; i < 255; i++) {
        addItem(Blocks::ROCK);
        addItem(Blocks::GRASS);
        addItem(Blocks::DIRT);
        addItem(Blocks::STONE);
        addItem(Blocks::PLANK);
        addItem(Blocks::WOOD);
        //addItem(Blocks::BEDROCK);TMD����ǻ���
        addItem(Blocks::LEAF);
        addItem(Blocks::GLASS);
        addItem(Blocks::WATER);
        addItem(Blocks::LAVA);
        addItem(Blocks::GLOWSTONE);
        addItem(Blocks::SAND);
        addItem(Blocks::CEMENT);
        addItem(Blocks::ICE);
        addItem(Blocks::COAL);
        addItem(Blocks::IRON);
        addItem(Blocks::TNT);
    }
}

void Player::updatePosition(Double3 velocity) {
    inWater = World::inWater(playerbox);
    if (!Flying && !CrossWall && inWater) {
        xa *= 0.6;
        ya *= 0.6;
        za *= 0.6;
    }
    const auto xal = velocity.X, yal = velocity.Y, zal = velocity.Z;
    
    if (ya != yal && yal < 0.0) {
        OnGround = true;
        Player::glidingEnergy = 0;
        Player::glidingSpeed = 0;
        Player::glidingNow = false;
        if (yal < -0.4 && Player::gamemode == Player::Survival) {
            Player::health += yal * Player::dropDamage;
        }
    } else OnGround = false;
    if (ya != yal && yal > 0.0) jump = 0.0;
    if (xa != xal || za != zal) NearWall = true; else NearWall = false;
    
    xa = static_cast<double>(static_cast<int>(xa * 100000)) / 100000.0;
    ya = static_cast<double>(static_cast<int>(ya * 100000)) / 100000.0;
    za = static_cast<double>(static_cast<int>(za * 100000)) / 100000.0;

    xd = xa;
    yd = ya;
    zd = za;
    xa *= 0.8;
    za *= 0.8;
    if (Flying || CrossWall) ya *= 0.8;
    if (OnGround) xa *= 0.7, ya = 0.0, za *= 0.7;
    updateHitbox();

    cxtl = cxt;
    cytl = cyt;
    cztl = czt;
    cxt = World::GetChunkPos(static_cast<int>(Pos.X));
    cyt = World::GetChunkPos(static_cast<int>(Pos.Y));
    czt = World::GetChunkPos(static_cast<int>(Pos.Z));
}

bool Player::putBlock(int x, int y, int z, Block blockname) {
    Hitbox::AABB blockbox;
    auto success = false;
    blockbox.xmin = x - 0.5;
    blockbox.ymin = y - 0.5;
    blockbox.zmin = z - 0.5;
    blockbox.xmax = x + 0.5;
    blockbox.ymax = y + 0.5;
    blockbox.zmax = z + 0.5;
    const auto cx = World::GetChunkPos(x), cy = World::GetChunkPos(y), cz = World::GetChunkPos(z);
    if (!World::ChunkOutOfBound({(cx), (cy), (cz)})
        && ((!Hitbox::Hit(playerbox, blockbox) || CrossWall ||
             !BlockInfo(blockname).isSolid()) && !BlockInfo(World::GetBlock({x, y, z})).isSolid())) {
        World::PutBlock({(x), (y), (z)}, blockname);
        success = true;
    }
    return success;
}

bool Player::save(std::string worldn) {
    uint32 curversion = VERSION;
    std::stringstream ss;
    ss << "Worlds/" << worldn << "/player.NEWorldPlayer";
    std::ofstream isave(ss.str().c_str(), std::ios::binary | std::ios::out);
    if (!isave.is_open()) return false;
    isave.write((char *) &curversion, sizeof(curversion));
	isave.write((char*)&Pos, sizeof(Pos));
    isave.write((char *) &lookupdown, sizeof(lookupdown));
    isave.write((char *) &heading, sizeof(heading));
    isave.write((char *) &jump, sizeof(jump));
    isave.write((char *) &OnGround, sizeof(OnGround));
    isave.write((char *) &Running, sizeof(Running));
    isave.write((char *) &AirJumps, sizeof(AirJumps));
    isave.write((char *) &Flying, sizeof(Flying));
    isave.write((char *) &CrossWall, sizeof(CrossWall));
    isave.write((char *) &indexInHand, sizeof(indexInHand));
    isave.write((char *) &health, sizeof(health));
    isave.write((char *) &gamemode, sizeof(gamemode));
    isave.write((char *) &gametime, sizeof(gametime));
    isave.write((char *) inventory, sizeof(inventory));
    isave.write((char *) inventoryAmount, sizeof(inventoryAmount));
    isave.close();
    return true;
}

bool Player::load(std::string worldn) {
    uint32 targetVersion;
    std::stringstream ss;
    ss << "Worlds/" << worldn << "/player.NEWorldPlayer";
    std::ifstream iload(ss.str().c_str(), std::ios::binary | std::ios::in);
    if (!iload.is_open()) return false;
    iload.read((char *) &targetVersion, sizeof(targetVersion));
    if (targetVersion != VERSION) return false;
	iload.read((char*)&Pos, sizeof(Pos));
    iload.read((char *) &lookupdown, sizeof(lookupdown));
    iload.read((char *) &heading, sizeof(heading));
    iload.read((char *) &jump, sizeof(jump));
    iload.read((char *) &OnGround, sizeof(OnGround));
    iload.read((char *) &Running, sizeof(Running));
    iload.read((char *) &AirJumps, sizeof(AirJumps));
    iload.read((char *) &Flying, sizeof(Flying));
    iload.read((char *) &CrossWall, sizeof(CrossWall));
    iload.read((char *) &indexInHand, sizeof(indexInHand));
    iload.read((char *) &health, sizeof(health));
    iload.read((char *) &gamemode, sizeof(gamemode));
    iload.read((char *) &gametime, sizeof(gametime));
    iload.read((char *) inventory, sizeof(inventory));
    iload.read((char *) inventoryAmount, sizeof(inventoryAmount));
    iload.close();
    return true;
}

bool Player::addItem(item itemname, short amount) {
    const auto InvMaxStack = 255;
    for (auto i = 3; i >= 0; i--) {
        for (auto j = 0; j != 10; j++) {
            if (inventory[i][j] == itemname && inventoryAmount[i][j] < InvMaxStack) {
                //�ҵ�һ��ͬ�����
                if (amount + inventoryAmount[i][j] <= InvMaxStack) {
                    inventoryAmount[i][j] += amount;
                    return true;
                } else {
                    amount -= InvMaxStack - inventoryAmount[i][j];
                    inventoryAmount[i][j] = InvMaxStack;
                }
            }
        }
    }
    for (auto i = 3; i >= 0; i--) {
        for (auto j = 0; j != 10; j++) {
            if (inventory[i][j] == Blocks::ENV) {
                //�ҵ�һ���հ׸���
                inventory[i][j] = itemname;
                if (amount <= InvMaxStack) {
                    inventoryAmount[i][j] = amount;
                    return true;
                } else {
                    inventoryAmount[i][j] = InvMaxStack;
                    amount -= InvMaxStack;
                }
            }
        }
    }
    return false;
}

void Player::changeGameMode(int _gamemode) {
    Player::gamemode = _gamemode;
    switch (_gamemode) {
        case Survival:
            Flying = false;
            Player::jump = 0.0;
            CrossWall = false;
            break;

        case Creative:
            Flying = true;
            Player::jump = 0.0;
            break;
    }
}

PlayerPacket Player::convertToPlayerPacket() {
    PlayerPacket p;
    p.x = Pos.X;
    p.y = Pos.Y + height + heightExt;
    p.z = Pos.Z;
    p.heading = heading;
    p.lookupdown = lookupdown;
    p.onlineID = onlineID;
    p.skinID = 0;
#ifdef NEWORLD_COMPILE_DISABLE_SECURE
    strcpy(p.name, name.c_str());
#else
    strcpy(p.name, name.c_str());
#endif
    return p;
}

