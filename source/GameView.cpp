#include "GameView.h"
#include "Blocks.h"
#include "Textures.h"
#include "GLProc.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Player.h"
#include "WorldGen.h"
#include "World.h"
#include "WorldRenderer.h"
#include "ShadowMaps.h"
#include "Particles.h"
#include "Hitbox.h"
#include "GUI.h"
#include "Menus.h"
#include "Frustum.h"
#include "Network.h"
#include "Effect.h"
#include "Items.h"
#include "Globalization.h"
#include "../PluginSDK/src/export_variables.h"
#include "../PluginSDK/src/pluginsdk.h"
#include "Setup.h"
#include "AudioSystem.h"

ThreadFunc UpdateThreadFunc(void*);

class GameDView : public GUI::Form
{
private:
    string chatword;
    bool chatmode = false;
    vector<string> chatMessages;

    int fps, fpsc, ups, upsc;
    double fctime, uctime;

    bool GUIrenderswitch;
    bool DebugMode;
    bool DebugHitbox;
    //bool DebugChunk;
    //bool DebugPerformance;
    bool DebugShadow;
    bool DebugMergeFace;

    bool updateThreadRun;

    int selx, sely, selz, oldselx, oldsely, oldselz, selface;
    bool sel;
    float selt, seldes;
    block selb;
    brightness selbr;
    bool selce;
    int selbx, selby, selbz, selcx, selcy, selcz;

    int GetMouseScroll()
    {
        return mw;
    }

    int GetMouseButton()
    {
        return mb;
    }

public:
    void GameThreadloop()
    {
        //Wait until start...
        MutexLock(Mutex);
        while (!updateThreadRun)
        {
            MutexUnlock(Mutex);
            Sleep(1);
            MutexLock(Mutex);
        }
        MutexUnlock(Mutex);

        //Thread start
        MutexLock(Mutex);
        lastupdate = timer();

        while (updateThreadRun)
        {
            MutexUnlock(Mutex);
            Sleep(1); //Don't make it always busy
            MutexLock(Mutex);

            while (updateThreadPaused)
            {
                MutexUnlock(Mutex);
                Sleep(1); //Same as before
                MutexLock(Mutex);
                lastupdate = updateTimer = timer();
            }

            FirstUpdateThisFrame = true;
            updateTimer = timer();
            if (updateTimer - lastupdate >= 5.0)
                lastupdate = updateTimer;

            while ((updateTimer - lastupdate) >= 1.0 / 30.0 && upsc < 60)
            {
                lastupdate += 1.0 / 30.0;
                upsc++;
                UpdateGame();
                FirstUpdateThisFrame = false;
            }

            if ((timer() - uctime) >= 1.0)
            {
                uctime = timer();
                ups = upsc;
                upsc = 0;
            }

        }
        MutexUnlock(Mutex);
    }

    void SaveGame()
    {
        World::saveAllChunks();
        if (!Player::save(World::WorldName))
        {
            DebugWarning("Failed saving player info!");
        }
    }

    bool LoadGame()
    {
        if (!Player::load(World::WorldName))
        {
            DebugWarning("Failed loading player info!");
            return false;
        }
        return true;
    }

    bool IsPressed(int key, bool setFalse = false)
    {
        static bool keyPressed[GLFW_KEY_LAST + 1];
        if (setFalse)
        {
            keyPressed[key] = false;
            return true;
        }
        if (key > GLFW_KEY_LAST || key <= 0)
            return false;
        if (!glfwGetKey(MainWindow, key))
            keyPressed[key] = false;
        if (!keyPressed[key] && glfwGetKey(MainWindow, key))
        {
            keyPressed[key] = true;
            return true;
        }
        return false;
    }

    void UpdateGame()
    {
        static double Wprstm;
        static bool WP;

        //用于音效更新
        bool BlockClick = false;
        ALfloat BlockPos[3];

        Player::BlockInHand = Player::inventory[3][Player::indexInHand];
        //生命值相关
        if (Player::health > 0 || Player::gamemode == Player::Creative)
        {
            if (Player::ypos < -100)
                Player::health -= ((-100) - Player::ypos) / 100;
            if (Player::health < Player::healthMax)
                Player::health += Player::healSpeed;
            if (Player::health > Player::healthMax)
                Player::health = Player::healthMax;
        }
        else
        {
            Player::spawn();
        }

        //时间
        gametime++;
        if (glfwGetKey(MainWindow, GLFW_KEY_F8))
            gametime += 30;
        if (gametime > MaxGameTime)
            gametime = 0;

        //World::unchunks.size()=0
        World::rebuiltChunks = 0;
        World::updatedChunks = 0;

        //cpArray move
        if (World::cpArray.originX != Player::cxt - ViewDistance - 2 || World::cpArray.originY != Player::cyt - ViewDistance - 2 || World::cpArray.originZ != Player::czt - ViewDistance - 2)
        {
            World::cpArray.moveTo(Player::cxt - ViewDistance - 2, Player::cyt - ViewDistance - 2, Player::czt - ViewDistance - 2);
        }
        //HeightMap move
        if (World::HMap.originX != (Player::cxt - ViewDistance - 2) * 16 || World::HMap.originZ != (Player::czt - ViewDistance - 2) * 16)
        {
            World::HMap.moveTo((Player::cxt - ViewDistance - 2) * 16, (Player::czt - ViewDistance - 2) * 16);
        }

        if (FirstUpdateThisFrame)
        {
            World::sortChunkLoadUnloadList(RoundInt(Player::xpos), RoundInt(Player::ypos), RoundInt(Player::zpos));

            //卸载区块(Unload chunks)
            int sumUnload = min(World::chunkUnloads, World::MaxChunkUnloads);
            for (int i = 0; i < sumUnload; i++)
            {
                World::chunk* cp = World::chunkUnloadList[i].first;
                assert(cp != World::EmptyChunkPtr);
                int cx = cp->cx, cy = cp->cy, cz = cp->cz;
                cp->Unload();
                World::DeleteChunk(cx, cy, cz);
            }

            //加载区块(Load chunks)
            int sumLoad = min(World::chunkLoads, World::MaxChunkLoads);
            for (int i = 0; i < sumLoad; i++)
            {
                int cx = World::chunkLoadList[i][1];
                int cy = World::chunkLoadList[i][2];
                int cz = World::chunkLoadList[i][3];
                World::chunk* c = World::AddChunk(cx, cy, cz);
                c->Load(false);
                if (c->Empty)
                {
                    c->Unload();
                    World::DeleteChunk(cx, cy, cz);
                    World::cpArray.setChunkPtr(cx, cy, cz, World::EmptyChunkPtr);
                }
            }

        }

        //加载动画
        for (auto cp : World::chunks)
        {
            if (cp->loadAnim <= 0.3f)
                cp->loadAnim = 0.0f;
            else
                cp->loadAnim *= 0.6f;
        }

        //随机状态更新
        for (auto cp : World::chunks)
        {
            int x, y, z, gx, gy, gz;
            int cx = cp->cx;
            int cy = cp->cy;
            int cz = cp->cz;
            x = mersenne->get_s32_ranged(0, 16);
            y = mersenne->get_s32_ranged(0, 16);
            z = mersenne->get_s32_ranged(0, 16);
            gx = x + (cx << 4);
            gy = y + (cy << 4);
            gz = z + (cz << 4);
            if (cp->getblock(x, y, z) == block(Blocks::DIRT) &&
                    World::getblock(gx, gy + 1, gz, Blocks::NONEMPTY) == block(Blocks::AIR) && (
                        World::getblock(gx + 1, gy, gz, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx - 1, gy, gz, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx, gy, gz + 1, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx, gy, gz - 1, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx + 1, gy + 1, gz, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx - 1, gy + 1, gz, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx, gy + 1, gz + 1, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx, gy + 1, gz - 1, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx + 1, gy - 1, gz, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx - 1, gy - 1, gz, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx, gy - 1, gz + 1, block(Blocks::AIR)) == block(Blocks::GRASS) ||
                        World::getblock(gx, gy - 1, gz - 1, block(Blocks::AIR)) == block(Blocks::GRASS)))
            {
                //长草
                cp->setblock(x, y, z, block(Blocks::GRASS));
                World::updateblock(gx, gy + 1, gz, true);
                World::setChunkUpdated(cx, cy, cz, true);
            }
        }
        World::ProcessBuq();

        //判断选中的方块
        double lx, ly, lz, lxl, lyl, lzl;
        lx = Player::xpos;
        ly = Player::ypos + Player::height + Player::heightExt;
        lz = Player::zpos;

        sel = false;
        selx = sely = selz = selbx = selby = selbz = selcx = selcy = selcz = selbr = 0;
        selb = block(0);

        if (!bagOpened)
        {
            //从玩家位置发射一条线段
            for (int i = 0; i < selectPrecision*selectDistance; i++)
            {
                lxl = lx;
                lyl = ly;
                lzl = lz;

                //线段延伸
                lx += sin(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) / (double)selectPrecision;
                ly += cos(M_PI / 180 * (Player::lookupdown + 90)) / (double)selectPrecision;
                lz += cos(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) / (double)selectPrecision;

                //碰到方块
                if (BlockInfo(World::getblock(RoundInt(lx), RoundInt(ly), RoundInt(lz))).isSolid())
                {
                    int x, y, z, xl, yl, zl;
                    x = RoundInt(lx);
                    y = RoundInt(ly);
                    z = RoundInt(lz);
                    xl = RoundInt(lxl);
                    yl = RoundInt(lyl);
                    zl = RoundInt(lzl);

                    selx = x;
                    sely = y;
                    selz = z;
                    sel = true;

                    //找方块所在区块及位置
                    selcx = getchunkpos(x);
                    selcy = getchunkpos(y);
                    selcz = getchunkpos(z);
                    selbx = getblockpos(x);
                    selby = getblockpos(y);
                    selbz = getblockpos(z);

                    if (World::chunkOutOfBound(selcx, selcy, selcz) == false)
                    {
                        World::chunk* cp = World::getChunkPtr(selcx, selcy, selcz);
                        if (cp == nullptr || cp == World::EmptyChunkPtr)
                            continue;
                        selb = cp->getblock(selbx, selby, selbz);
                    }

                    selbr = World::getbrightness(xl, yl, zl);
                    selb = World::getblock(x, y, z);

                    if (mb == 1 || glfwGetKey(MainWindow, GLFW_KEY_ENTER) == GLFW_PRESS)
                    {
                        Particles::throwParticle(selb,
                                                 float(x + rnd() - 0.5f), float(y + rnd() - 0.2f), float(z + rnd() - 0.5f),
                                                 float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f),
                                                 float(rnd()*0.01f + 0.02f), int(rnd() * 30) + 30);

                        if (selx != oldselx || sely != oldsely || selz != oldselz)
                        {
                            seldes = 0.0;
                        }
                        else
                        {
                            float Factor = 1.0f;
                            if (Player::inventory[3][Player::indexInHand] == STICK)
                            {
                                Factor = 4.0;
                            }
                            else
                            {
                                Factor = 30.0f / (BlockInfo(Player::inventory[3][Player::indexInHand]).getHardness() + 0.1f);
                                if (Factor < 1.0f)
                                    Factor = 1.0f;
                                if (Factor > 1.7f)
                                    Factor = 1.7f;
                            }
                            seldes += BlockInfo(selb).getHardness()*((Player::gamemode == Player::Creative) ? 10.0f : 0.3f)*Factor;
                            BlockClick = true;
                            BlockPos[0] = static_cast<ALfloat>(x);
                            BlockPos[1] = static_cast<ALfloat>(y);
                            BlockPos[2] = static_cast<ALfloat>(z);

                        }

                        if (seldes >= 100.0)
                        {
                            for (int j = 1; j <= 25; j++)
                            {
                                Particles::throwParticle(selb,
                                                         float(x + rnd() - 0.5f), float(y + rnd() - 0.2f), float(z + rnd() - 0.5f),
                                                         float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f), float(rnd()*0.2f - 0.1f),
                                                         float(rnd()*0.02 + 0.03), int(rnd() * 60) + 30);
                            }
                            World::pickblock(x, y, z);
                            BlockClick = true;
                            BlockPos[0] = static_cast<ALfloat>(x);
                            BlockPos[1] = static_cast<ALfloat>(y);
                            BlockPos[2] = static_cast<ALfloat>(z);
                        }
                    }

                    if (((mb == 2 && mbp == false) || (!chatmode&&IsPressed(GLFW_KEY_TAB))))   //鼠标右键
                    {
                        if (Player::inventoryAmount[3][Player::indexInHand] > 0 && isBlock(Player::inventory[3][Player::indexInHand]))
                        {
                            //放置方块
                            if (Player::putBlock(xl, yl, zl, Player::BlockInHand))
                            {
                                Player::inventoryAmount[3][Player::indexInHand]--;
                                if (Player::inventoryAmount[3][Player::indexInHand] == 0)
                                    Player::inventory[3][Player::indexInHand] = block(Blocks::AIR);
                                BlockClick = true;
                                BlockPos[0] = static_cast<ALfloat>(x);
                                BlockPos[1] = static_cast<ALfloat>(y);
                                BlockPos[2] = static_cast<ALfloat>(z);
                            }
                        }
                        else
                        {
                            //使用物品
                            if (Player::inventory[3][Player::indexInHand] == APPLE)
                            {
                                Player::inventoryAmount[3][Player::indexInHand]--;
                                if (Player::inventoryAmount[3][Player::indexInHand] == 0)
                                    Player::inventory[3][Player::indexInHand] = block(Blocks::AIR);
                                Player::health = Player::healthMax;
                            }
                        }
                    }
                    break;
                }
            }

            if (selx != oldselx || sely != oldsely || selz != oldselz || (mb == 0 && glfwGetKey(MainWindow, GLFW_KEY_ENTER) != GLFW_PRESS))
                seldes = 0.0;
            oldselx = selx;
            oldsely = sely;
            oldselz = selz;

            Player::intxpos = RoundInt(Player::xpos);
            Player::intypos = RoundInt(Player::ypos);
            Player::intzpos = RoundInt(Player::zpos);

            //更新方向
            Player::heading += Player::xlookspeed;
            Player::lookupdown += Player::ylookspeed;
            Player::xlookspeed = Player::ylookspeed = 0.0;

            if (!chatmode)
            {
                //移动！(生命在于运动)
                if (glfwGetKey(MainWindow, GLFW_KEY_W) || Player::glidingNow)
                {
                    if (!WP)
                    {
                        if (Wprstm == 0.0)
                        {
                            Wprstm = timer();
                        }
                        else
                        {
                            if (timer() - Wprstm <= 0.5)
                            {
                                Player::Running = true;
                                Wprstm = 0.0;
                            }
                            else
                            {
                                Wprstm = timer();
                            }
                        }
                    }
                    if (Wprstm != 0.0 && timer() - Wprstm > 0.5)
                        Wprstm = 0.0;
                    WP = true;
                    if (!Player::glidingNow)
                    {
                        Player::xa += -sin(Player::heading*M_PI / 180.0) * Player::speed;
                        Player::za += -cos(Player::heading*M_PI / 180.0) * Player::speed;
                    }
                    else
                    {
                        Player::xa = sin(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) * Player::glidingSpeed * speedCast;
                        Player::ya = cos(M_PI / 180 * (Player::lookupdown + 90)) * Player::glidingSpeed * speedCast;
                        Player::za = cos(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) * Player::glidingSpeed * speedCast;
                        if (Player::ya < 0) Player::ya *= 2;
                    }
                }
                else
                {
                    Player::Running = false;
                    WP = false;
                }

                Player::speed = (Player::Running ? runspeed : walkspeed);

                if (glfwGetKey(MainWindow, GLFW_KEY_S) == GLFW_PRESS&&!Player::glidingNow)
                {
                    Player::xa += sin(Player::heading*M_PI / 180.0) * Player::speed;
                    Player::za += cos(Player::heading*M_PI / 180.0) * Player::speed;
                    Wprstm = 0.0;
                }

                if (glfwGetKey(MainWindow, GLFW_KEY_A) == GLFW_PRESS&&!Player::glidingNow)
                {
                    Player::xa += sin((Player::heading - 90)*M_PI / 180.0) * Player::speed;
                    Player::za += cos((Player::heading - 90)*M_PI / 180.0) * Player::speed;
                    Wprstm = 0.0;
                }

                if (glfwGetKey(MainWindow, GLFW_KEY_D) == GLFW_PRESS&&!Player::glidingNow)
                {
                    Player::xa += -sin((Player::heading - 90)*M_PI / 180.0) * Player::speed;
                    Player::za += -cos((Player::heading - 90)*M_PI / 180.0) * Player::speed;
                    Wprstm = 0.0;
                }

                if (!Player::Flying && !Player::CrossWall)
                {
                    double horizontalSpeed = sqrt(Player::xa*Player::xa + Player::za*Player::za);
                    if (horizontalSpeed > Player::speed && !Player::glidingNow)
                    {
                        Player::xa *= Player::speed / horizontalSpeed;
                        Player::za *= Player::speed / horizontalSpeed;
                    }
                }
                else
                {
                    if (glfwGetKey(MainWindow, GLFW_KEY_R) == GLFW_PRESS && !Player::glidingNow)
                    {
                        if (glfwGetKey(MainWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                        {
                            Player::xa = -sin(Player::heading*M_PI / 180.0) * runspeed * 10;
                            Player::za = -cos(Player::heading*M_PI / 180.0) * runspeed * 10;
                        }
                        else
                        {
                            Player::xa = sin(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                            Player::ya = cos(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                            Player::za = cos(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                        }
                    }

                    if (glfwGetKey(MainWindow, GLFW_KEY_F) == GLFW_PRESS && !Player::glidingNow)
                    {
                        if (glfwGetKey(MainWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                        {
                            Player::xa = sin(Player::heading*M_PI / 180.0) * runspeed * 10;
                            Player::za = cos(Player::heading*M_PI / 180.0) * runspeed * 10;
                        }
                        else
                        {
                            Player::xa = -sin(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                            Player::ya = -cos(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                            Player::za = -cos(M_PI / 180 * (Player::heading - 180))*sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                        }
                    }
                }

                //切换方块
                if (IsPressed(GLFW_KEY_Z) && Player::indexInHand > 0)
                    Player::indexInHand--;
                if (IsPressed(GLFW_KEY_X) && Player::indexInHand < 9)
                    Player::indexInHand++;
                if ((int)Player::indexInHand + (mwl - mw) < 0)
                    Player::indexInHand = 0;
                else if ((int)Player::indexInHand + (mwl - mw) > 9)
                    Player::indexInHand = 9;
                else
                    Player::indexInHand += (char)(mwl - mw);
                mwl = mw;

                //起跳！
                if (IsPressed(GLFW_KEY_SPACE))
                {
                    if (!Player::inWater)
                    {
                        if ((Player::OnGround || Player::AirJumps < MaxAirJumps) && Player::Flying == false && Player::CrossWall == false)
                        {
                            if (Player::OnGround == false)
                            {
                                Player::jump = 0.3;
                                Player::AirJumps++;
                            }
                            else
                            {
                                Player::jump = 0.25;
                                Player::OnGround = false;
                            }
                        }
                        if (Player::Flying || Player::CrossWall)
                        {
                            Player::ya += walkspeed / 2;
                            IsPressed(GLFW_KEY_SPACE, true);
                        }
                        Wprstm = 0.0;
                    }
                    else
                    {
                        Player::ya = walkspeed;
                        IsPressed(GLFW_KEY_SPACE, true);
                    }
                }

                if ((glfwGetKey(MainWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(MainWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) && !Player::glidingNow)
                {
                    if (Player::CrossWall || Player::Flying)
                        Player::ya -= walkspeed / 2;
                    Wprstm = 0.0;
                }

                if (glfwGetKey(MainWindow, GLFW_KEY_K) && Player::Glide && !Player::OnGround && !Player::glidingNow)
                {
                    Player::glidingEnergy = g * (Player::ypos + Player::height + Player::heightExt);
                    Player::glidingSpeed = 0;
                    Player::glidingNow = true;
                }

                //各种设置切换
                if (IsPressed(GLFW_KEY_F1))
                    Player::changeGameMode(Player::gamemode == Player::Creative ? Player::Survival : Player::Creative);
                if (IsPressed(GLFW_KEY_F2))
                    shouldGetScreenshot = true;
                if (IsPressed(GLFW_KEY_F3))
                    DebugMode = !DebugMode;
                if (IsPressed(GLFW_KEY_F4))
                    Player::CrossWall = !Player::CrossWall;
                if (IsPressed(GLFW_KEY_H) && glfwGetKey(MainWindow, GLFW_KEY_F3) == GLFW_PRESS)
                {
                    DebugHitbox = !DebugHitbox;
                    DebugMode = true;
                }
                if (Renderer::AdvancedRender)
                {
                    if (IsPressed(GLFW_KEY_M) && glfwGetKey(MainWindow, GLFW_KEY_F3) == GLFW_PRESS)
                    {
                        DebugShadow = !DebugShadow;
                        DebugMode = true;
                    }
                }
                else
                {
                    DebugShadow = false;
                }
                if (IsPressed(GLFW_KEY_G) && glfwGetKey(MainWindow, GLFW_KEY_F3) == GLFW_PRESS)
                {
                    DebugMergeFace = !DebugMergeFace;
                    DebugMode = true;
                }
                if (IsPressed(GLFW_KEY_F4 && Player::gamemode == Player::Creative))
                    Player::CrossWall = !Player::CrossWall;
                if (IsPressed(GLFW_KEY_F5))
                    GUIrenderswitch = !GUIrenderswitch;
                if (IsPressed(GLFW_KEY_F6))
                    Player::Glide = !Player::Glide;
                if (IsPressed(GLFW_KEY_F7))
                    Player::spawn();
                if (IsPressed(GLFW_KEY_SLASH))
                    chatmode = true; //斜杠将会在下面的if(chatmode)里添加
            }

            if (IsPressed(GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                chatmode = !chatmode;
                if (chatword.size() > 0)   //指令的执行，或发出聊天文本
                {
                    if (chatword[0] == '/')   //指令
                    {
                        vector<string> command = split(chatword, " ");
                        if (!ExecuteCommand(command))   //执行失败
                        {
                            DebugWarning("Fail to execute the command: " + chatword);
                            chatMessages.push_back("Fail to execute the command: " + chatword);
                        }
                    }
                    else
                    {
                        chatMessages.push_back(chatword);
                    }
                }
                chatword = "";
            }

            if (chatmode)
            {
                if (IsPressed(GLFW_KEY_BACKSPACE) && chatword.length()>0)
                {
                    int n = chatword[chatword.length() - 1];
                    if (n > 0 && n <= 127)
                        chatword = chatword.substr(0, chatword.length() - 1);
                    else
                        chatword = chatword.substr(0, chatword.length() - 2);
                }
                else
                {
                    chatword += inputstr;
                }
                //自动补全
                if (IsPressed(GLFW_KEY_TAB) && chatmode && chatword.size() > 0 && chatword.substr(0, 1) == "/")
                {
#ifdef NEWORLD_TARGET_WINDOWS
                    for (std::map<std::wstring, command>::iterator it = commands.begin(); it != commands.end(); it++)
                        if (beginWith(it->first, StringToWString(chatword)))
                            chatword = WStringToString(it->first);
#endif
                }
            }
        }

        inputstr = "";

        if (IsPressed(GLFW_KEY_E) && GUIrenderswitch && !chatmode)
        {
            bagOpened = !bagOpened;
            bagAnimTimer = timer();
            if (!bagOpened)
            {
                glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else
            {
                shouldGetThumbnail = true;
                Player::xlookspeed = Player::ylookspeed = 0.0;
            }
        }

        if (!bagOpened && !chatmode)
        {
            if (IsPressed(GLFW_KEY_L))
                World::saveAllChunks();
        }

        //跳跃
        if (!Player::glidingNow)
        {
            if (!Player::inWater)
            {
                if (!Player::Flying && !Player::CrossWall)
                {
                    Player::ya = -0.001;
                    if (Player::OnGround)
                    {
                        Player::jump = 0.0;
                        Player::AirJumps = 0;
                        IsPressed(GLFW_KEY_SPACE, true);
                    }
                    else
                    {
                        //自由落体计算
                        Player::jump -= 0.025;
                        Player::ya = Player::jump + 0.5 * 0.6 / 900.0;
                    }
                }
                else
                {
                    Player::jump = 0.0;
                    Player::AirJumps = 0;
                }
            }
            else
            {
                Player::jump = 0.0;
                Player::AirJumps = MaxAirJumps;
                IsPressed(GLFW_KEY_SPACE, true);
                if (Player::ya <= 0.001 && !Player::Flying && !Player::CrossWall)
                {
                    Player::ya = -0.001;
                    if (!Player::OnGround) Player::ya -= 0.1;
                }
            }
        }

        if (Player::glidingNow)
        {
            double& E = Player::glidingEnergy;
            double oldh = Player::ypos + Player::height + Player::heightExt + Player::ya;
            double h = oldh;
            if (E - Player::glidingMinimumSpeed < h*g)    //小于最小速度
            {
                h = (E - Player::glidingMinimumSpeed) / g;
            }
            Player::glidingSpeed = sqrt(2 * (E - g*h));
            E -= EDrop;
            Player::ya += h - oldh;
        }

        //音效更新
        int Run = 0;
        if (WP)Run = Player::Running ? 2 : 1;
        ALfloat PlayerPos[3];
        PlayerPos[0] = static_cast<ALfloat>(Player::xpos);
        PlayerPos[1] = static_cast<ALfloat>(Player::ypos);
        PlayerPos[2] = static_cast<ALfloat>(Player::zpos);
        bool Fall = Player::OnGround && (!Player::inWater) && (Player::jump == 0);
        //更新声速
        AudioSystem::SpeedOfSound = Player::inWater ? AudioSystem::Water_SpeedOfSound : AudioSystem::Air_SpeedOfSound;
#ifdef NEWORLD_TARGET_WINDOWS
        //更新环境
        if (Player::inWater)
        {
            EFX::EAXprop = UnderWater;
        }
        else
        {
            if (Player::OnGround)
            {
                EFX::EAXprop = Plain;
            }
            else
            {
                EFX::EAXprop = Generic;
            }
        }
        EFX::UpdateEAXprop();
#endif
        AudioSystem::Update(PlayerPos, Fall, BlockClick, BlockPos, Run, Player::inWater);

        mbp = mb;
        FirstFrameThisUpdate = true;
        Particles::updateall();

        Player::intxpos = RoundInt(Player::xpos);
        Player::intypos = RoundInt(Player::ypos);
        Player::intzpos = RoundInt(Player::zpos);
        Player::updatePosition();
        Player::xposold = Player::xpos;
        Player::yposold = Player::ypos;
        Player::zposold = Player::zpos;
        Player::intxposold = RoundInt(Player::xpos);
        Player::intyposold = RoundInt(Player::ypos);
        Player::intzposold = RoundInt(Player::zpos);
    }

    void Grender()
    {
        //画场景
        double curtime = timer();
        double TimeDelta;
        double xpos, ypos, zpos;
        int renderedChunk = 0;

        //检测帧速率
        if (timer() - fctime >= 1.0)
        {
            fps = fpsc;
            fpsc = 0;
            fctime = timer();
        }
        fpsc++;

        lastframe = curtime;

        if (Player::Running)
        {
            if (FOVyExt < 9.8)
            {
                TimeDelta = curtime - SpeedupAnimTimer;
                FOVyExt = 10.0f - (10.0f - FOVyExt)*(float)pow(0.8, TimeDelta * 30);
                SpeedupAnimTimer = curtime;
            }
            else FOVyExt = 10.0;
        }
        else
        {
            if (FOVyExt > 0.2)
            {
                TimeDelta = curtime - SpeedupAnimTimer;
                FOVyExt *= (float)pow(0.8, TimeDelta * 30);
                SpeedupAnimTimer = curtime;
            }
            else FOVyExt = 0.0;
        }
        SpeedupAnimTimer = curtime;

        if (Player::OnGround)
        {
            //半蹲特效
            if (Player::jump < -0.005)
            {
                if (Player::jump <= -(Player::height - 0.5f))
                    Player::heightExt = -(Player::height - 0.5f);
                else
                    Player::heightExt = (float)Player::jump;
                TouchdownAnimTimer = curtime;
            }
            else
            {
                if (Player::heightExt <= -0.005)
                {
                    Player::heightExt *= (float)pow(0.8, (curtime - TouchdownAnimTimer) * 30);
                    TouchdownAnimTimer = curtime;
                }
            }
        }

        xpos = Player::xpos - Player::xd + (curtime - lastupdate) * 30.0 * Player::xd;
        ypos = Player::ypos + Player::height + Player::heightExt - Player::yd + (curtime - lastupdate) * 30.0 * Player::yd;
        zpos = Player::zpos - Player::zd + (curtime - lastupdate) * 30.0 * Player::zd;

        if (!bagOpened)
        {
            //转头！你治好了我多年的颈椎病！
            if (mx != mxl)
                Player::xlookspeed -= (mx - mxl)*mousemove;
            if (my != myl)
                Player::ylookspeed += (my - myl)*mousemove;
            if (glfwGetKey(MainWindow, GLFW_KEY_RIGHT) == 1)
                Player::xlookspeed -= mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_LEFT) == 1)
                Player::xlookspeed += mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_UP) == 1)
                Player::ylookspeed -= mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_DOWN) == 1)
                Player::ylookspeed += mousemove * 16 * (curtime - lastframe) * 30.0;
            //限制角度，别把头转掉下来了 ←_←
            if (Player::lookupdown + Player::ylookspeed < -90.0)
                Player::ylookspeed = -90.0 - Player::lookupdown;
            if (Player::lookupdown + Player::ylookspeed > 90.0)
                Player::ylookspeed = 90.0 - Player::lookupdown;
        }

        Player::cxt = getchunkpos((int)Player::xpos);
        Player::cyt = getchunkpos((int)Player::ypos);
        Player::czt = getchunkpos((int)Player::zpos);

        //更新区块VBO
        World::sortChunkBuildRenderList(RoundInt(Player::xpos), RoundInt(Player::ypos), RoundInt(Player::zpos));
        int brl = World::chunkBuildRenders > World::MaxChunkRenders ? World::MaxChunkRenders : World::chunkBuildRenders;
        for (int i = 0; i < brl; i++)
            World::chunks[World::chunkBuildRenderList[i][1]]->buildRender();

        //删除已卸载区块的VBO
        if (World::vbuffersShouldDelete.size() > 0)
        {
            glDeleteBuffersARB(World::vbuffersShouldDelete.size(), World::vbuffersShouldDelete.data());
            World::vbuffersShouldDelete.clear();
        }

        glFlush();

        double plookupdown = Player::lookupdown + Player::ylookspeed;
        double pheading = Player::heading + Player::xlookspeed;

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        //daylight = clamp((1.0 - cos((double)gametime / MaxGameTime * 2.0 * M_PI) * 2.0) / 2.0, 0.05, 1.0);
        //Renderer::sunlightXrot = 90 * daylight;
        if (Renderer::AdvancedRender)
        {
            //Build shadow map
            if (!DebugShadow) ShadowMaps::BuildShadowMap(xpos, ypos, zpos, curtime);
            else ShadowMaps::RenderShadowMap(xpos, ypos, zpos, curtime);
        }
        glClearColor(skycolorR, skycolorG, skycolorB, 1.0);
        if (!DebugShadow)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        Player::ViewFrustum.LoadIdentity();
        Player::ViewFrustum.SetPerspective(FOVyNormal + FOVyExt, (float)windowwidth / windowheight, 0.05f, ViewDistance * 16.0f);
        Player::ViewFrustum.MultRotate((float)plookupdown, 1, 0, 0);
        Player::ViewFrustum.MultRotate(360.0f - (float)pheading, 0, 1, 0);
        Player::ViewFrustum.update();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMultMatrixf(Player::ViewFrustum.getProjMatrix());
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotated(plookupdown, 1, 0, 0);
        glRotated(360.0 - pheading, 0, 1, 0);

        World::calcVisible(xpos, ypos, zpos, Player::ViewFrustum);
        renderedChunk = WorldRenderer::ListRenderChunks(Player::cxt, Player::cyt, Player::czt, ViewDistance, curtime);

        MutexUnlock(Mutex);

        if (MergeFace)
        {
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, BlockTextures3D);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, BlockTextures);
        }

        if (DebugMergeFace)
        {
            glDisable(GL_LINE_SMOOTH);
            glPolygonMode(GL_FRONT, GL_LINE);
        }

        glDisable(GL_BLEND);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);

        if (Renderer::AdvancedRender)
            Renderer::EnableShaders();
        if (!DebugShadow)
            WorldRenderer::RenderChunks(xpos, ypos, zpos, 0);
        if (Renderer::AdvancedRender)
            Renderer::DisableShaders();

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        if (MergeFace)
        {
            glDisable(GL_TEXTURE_3D);
            glEnable(GL_TEXTURE_2D);
        }
        glEnable(GL_BLEND);

        if (DebugMergeFace)
        {
            glEnable(GL_LINE_SMOOTH);
            glPolygonMode(GL_FRONT, GL_FILL);
        }

        MutexLock(Mutex);

        if (seldes > 0.0)
        {
            glTranslated(selx - xpos, sely - ypos, selz - zpos);
            RenderDestroy(seldes, 0, 0, 0);
            glTranslated(-selx + xpos, -sely + ypos, -selz + zpos);
        }
        glBindTexture(GL_TEXTURE_2D, BlockTextures);
        Particles::renderall(xpos, ypos, zpos);

        glDisable(GL_TEXTURE_2D);
        if (GUIrenderswitch&&sel)
        {
            glTranslated(selx - xpos, sely - ypos, selz - zpos);
            DrawBorder(0, 0, 0);
            glTranslated(-selx + xpos, -sely + ypos, -selz + zpos);
        }

        MutexUnlock(Mutex);

        glLoadIdentity();
        glRotated(plookupdown, 1, 0, 0);
        glRotated(360.0 - pheading, 0, 1, 0);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);

        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);

        if (Renderer::AdvancedRender)
            Renderer::EnableShaders();

        if (MergeFace)
        {
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, BlockTextures3D);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, BlockTextures);
        }

        if (DebugMergeFace)
        {
            glDisable(GL_LINE_SMOOTH);
            glPolygonMode(GL_FRONT, GL_LINE);
        }

        if (!DebugShadow)
            WorldRenderer::RenderChunks(xpos, ypos, zpos, 1);
        glDisable(GL_CULL_FACE);
        if (!DebugShadow)
            WorldRenderer::RenderChunks(xpos, ypos, zpos, 2);
        if (Renderer::AdvancedRender)
            Renderer::DisableShaders();

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        if (MergeFace)
        {
            glDisable(GL_TEXTURE_3D);
            glEnable(GL_TEXTURE_2D);
        }

        if (DebugMergeFace)
        {
            glEnable(GL_LINE_SMOOTH);
            glPolygonMode(GL_FRONT, GL_FILL);
        }

        glLoadIdentity();
        glRotated(plookupdown, 1, 0, 0);
        glRotated(360.0 - pheading, 0, 1, 0);
        glTranslated(-xpos, -ypos, -zpos);

        MutexLock(Mutex);

        if (DebugHitbox)
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_TEXTURE_2D);
            for (unsigned int i = 0; i < Player::Hitboxes.size(); i++)
            {
                Hitbox::renderAABB(Player::Hitboxes[i], GUI::FgR, GUI::FgG, GUI::FgB, 3, 0.002);
            }

            glLoadIdentity();
            glRotated(plookupdown, 1, 0, 0);
            glRotated(360.0 - pheading, 0, 1, 0);
            glTranslated(-Player::xpos, -Player::ypos - Player::height - Player::heightExt, -Player::zpos);

            Hitbox::renderAABB(Player::playerbox, 1.0f, 1.0f, 1.0f, 1);
            Hitbox::renderAABB(Hitbox::Expand(Player::playerbox, Player::xd, Player::yd, Player::zd), 1.0f, 1.0f, 1.0f, 1);
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, windowwidth, windowheight, 0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (World::getblock(RoundInt(xpos), RoundInt(ypos), RoundInt(zpos)) == block(Blocks::WATER))
        {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, BlockTextures);
            double tcX = Textures::getTexcoordX(block(Blocks::WATER), 1);
            double tcY = Textures::getTexcoordY(block(Blocks::WATER), 1);
            glBegin(GL_QUADS);
            glTexCoord2d(tcX, tcY + 1 / 8.0);
            glVertex2i(0, 0);
            glTexCoord2d(tcX, tcY);
            glVertex2i(0, windowheight);
            glTexCoord2d(tcX + 1 / 8.0, tcY);
            glVertex2i(windowwidth, windowheight);
            glTexCoord2d(tcX + 1 / 8.0, tcY + 1 / 8.0);
            glVertex2i(windowwidth, 0);
            glEnd();
        }
        if (GUIrenderswitch)
        {
            DrawGUI();
            DrawBag();
        }

        glDisable(GL_TEXTURE_2D);
        if (curtime - screenshotAnimTimer <= 1.0 && !shouldGetScreenshot)
        {
            float col = 1.0f - (float)(curtime - screenshotAnimTimer);
            glColor4f(1.0f, 1.0f, 1.0f, col);
            glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, windowheight);
            glVertex2i(windowwidth, windowheight);
            glVertex2i(windowwidth, 0);
            glEnd();
        }
        glEnable(GL_TEXTURE_2D);

        if (shouldGetScreenshot)
        {
            shouldGetScreenshot = false;
            screenshotAnimTimer = curtime;
            time_t t = time(0);
            char tmp[64];
            tm timeinfo;
#ifdef NEWORLD_COMPILE_DISABLE_SECURE
            timeinfo = *localtime(&t);
#else
#ifdef NEWORLD_TARGET_MACOSX
            timeinfo = *localtime(&t);
#else
            localtime_s(&timeinfo, &t);
#endif
#endif
            strftime(tmp, sizeof(tmp), "%Y年%m月%d日%H时%M分%S秒", &timeinfo);
            std::stringstream ss;
            ss << "Screenshots/" << tmp << ".bmp";
            SaveScreenshot(0, 0, windowwidth, windowheight, ss.str());
        }
        if (shouldGetThumbnail)
        {
            shouldGetThumbnail = false;
            CreateThumbnail();
        }

        //屏幕刷新，千万别删，后果自负！！！
        //====refresh====//
        MutexUnlock(Mutex);
    }

    void onRender()
    {
        MutexLock(Mutex);
        //==refresh end==//
    }

    void DrawBorder(int x, int y, int z)
    {
        //绘制选择边框
        static float eps = 0.002f; //实际上这个边框应该比方块大一些，否则很难看
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1);
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_LINES);
        // Left Face
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        // Front Face
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        // Right Face
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        // Back Face
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glEnd();
        glDisable(GL_LINE_SMOOTH);
    }

    void DrawGUI()
    {
        int windowuswidth = getStretchedIntWindowWidth(), windowusheight = getStretchedIntWindowHeight();
        glDepthFunc(GL_ALWAYS);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LINE_SMOOTH);
        float seldes_100 = seldes / 100.0f;
        int disti = (int)(seldes_100*linedist);

        if (DebugMode)
        {

            if (selb != block(Blocks::AIR))
            {
                glLineWidth(1);
                glBegin(GL_LINES);
                glColor4f(GUI::FgR, GUI::FgG, GUI::FgB, 0.8f);
                UIVertex(windowuswidth / 2, windowusheight / 2);
                UIVertex(windowuswidth / 2 + 50, windowusheight / 2 + 50);
                UIVertex(windowuswidth / 2 + 50, windowusheight / 2 + 50);
                UIVertex(windowuswidth / 2 + 250, windowusheight / 2 + 50);
                glEnd();
                TextRenderer::setFontColor(1.0f, 1.0f, 1.0f, 0.8f);
                glEnable(GL_TEXTURE_2D);
                glDisable(GL_CULL_FACE);
                std::stringstream ss;
                ss << BlockInfo(selb).getBlockName() << " (ID " << selb.ID << ":" << selb.Data81 << ")";
                TextRenderer::renderString(windowuswidth / 2 + 50, windowusheight / 2 + 50 - 16, ss.str());
                glDisable(GL_TEXTURE_2D);
                glEnable(GL_CULL_FACE);
                glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
            }
            else
            {
                glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
            }

            glLineWidth(2);

            glBegin(GL_LINES);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 - linedist + linelength + disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 - linedist + linelength + disti, windowusheight / 2 - linedist + disti);

            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 - linedist + linelength + disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 + linedist - linelength - disti, windowusheight / 2 - linedist + disti);

            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 + linedist - linelength - disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 - linedist + linelength + disti, windowusheight / 2 + linedist - disti);

            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 + linedist - linelength - disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 + linedist - linelength - disti, windowusheight / 2 + linedist - disti);

            glEnd();

        }

        glLineWidth(4.0f * static_cast<float>(stretch));
        glBegin(GL_LINES);
        glColor4f(0.0, 0.0, 0.0, 1.0);
        UIVertex(windowuswidth / 2 - 16, windowusheight / 2);
        UIVertex(windowuswidth / 2 + 16, windowusheight / 2);
        UIVertex(windowuswidth / 2, windowusheight / 2 - 16);
        UIVertex(windowuswidth / 2, windowusheight / 2 + 16);
        glEnd();
        glLineWidth(2.0f * static_cast<float>(stretch));
        glBegin(GL_LINES);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        UIVertex(windowuswidth / 2 - 15, windowusheight / 2);
        UIVertex(windowuswidth / 2 + 15, windowusheight / 2);
        UIVertex(windowuswidth / 2, windowusheight / 2 - 15);
        UIVertex(windowuswidth / 2, windowusheight / 2 + 15);
        glEnd();

        if (seldes > 0.0f)
        {

            glBegin(GL_LINES);
            glColor4f(0.5, 0.5, 0.5, 1.0);
            glVertex2i(windowwidth / 2 - 15, windowheight / 2);
            glVertex2i(windowwidth / 2 - 15 + (int)(seldes_100 * 15), windowheight / 2);
            glVertex2i(windowwidth / 2 + 15, windowheight / 2);
            glVertex2i(windowwidth / 2 + 15 - (int)(seldes_100 * 15), windowheight / 2);
            glVertex2i(windowwidth / 2, windowheight / 2 - 15);
            glVertex2i(windowwidth / 2, windowheight / 2 - 15 + (int)(seldes_100 * 15));
            glVertex2i(windowwidth / 2, windowheight / 2 + 15);
            glVertex2i(windowwidth / 2, windowheight / 2 + 15 - (int)(seldes_100 * 15));
            glEnd();

        }

        glDisable(GL_CULL_FACE);

        if (Player::gamemode == Player::Survival)
        {
            glColor4d(0.8, 0.0, 0.0, 0.3);
            glBegin(GL_QUADS);
            UIVertex(10, 10);
            UIVertex(200, 10);
            UIVertex(200, 30);
            UIVertex(10, 30);
            glEnd();

            double healthPercent = (double)Player::health / Player::healthMax;
            glColor4d(1.0, 0.0, 0.0, 0.5);
            glBegin(GL_QUADS);
            UIVertex(20, 15);
            UIVertex(static_cast<int>(20 + healthPercent * 170), 15);
            UIVertex(static_cast<int>(20 + healthPercent * 170), 25);
            UIVertex(20, 25);
            glEnd();
        }

        TextRenderer::setFontColor(1.0f, 1.0f, 1.0f, 0.9f);
        if (chatmode)
        {
            glColor4f(GUI::FgR, GUI::FgG, GUI::FgB, GUI::FgA);
            glDisable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
            glVertex2i(1, windowheight - 33);
            glVertex2i(windowwidth - 1, windowheight - 33);
            glVertex2i(windowwidth - 1, windowheight - 51);
            glVertex2i(1, windowheight - 51);
            glEnd();
            glEnable(GL_TEXTURE_2D);
            TextRenderer::renderString(0, windowheight - 50, chatword);
        }
        int posy = 0;
        int size = chatMessages.size();
        if (size != 0)
        {
            for (int i = size - 1; i >= (size - 10 > 0 ? size - 10 : 0); --i)
            {
                TextRenderer::renderString(0, windowheight - 80 - 18 * posy++, chatMessages[i]);
            }
        }

        //if (DebugShadow) ShadowMaps::DrawShadowMap(windowwidth / 2, windowheight / 2, windowwidth, windowheight);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextRenderer::Font);

        if (DebugMode)
        {
            std::stringstream ss;
            int pos = 0;
            TextRenderer::renderASCIIString(0, (pos++) * 16, "NEWorld v" + pack(version) + "[OpenGL " + pack(gl_version_major) + "." + pack(gl_version_minor) + "|" + pack(gl_version_rev) + "]");
            TextRenderer::renderASCIIString(0, (pos++) * 16, "Fps:" + pack(fps) + "|" + "Ups:" + pack(ups));

            TextRenderer::renderASCIIString(0, (pos++) * 16, "Debug Mode:" + pack(DebugMode));
            if (Renderer::AdvancedRender)
                TextRenderer::renderASCIIString(0, (pos++) * 16, "Shadow View:" + pack(DebugShadow));
            TextRenderer::renderASCIIString(0, (pos++) * 16, "X:" + pack(Player::xpos) + " Y:" + pack(Player::ypos) + " Z:" + pack(Player::zpos));
            TextRenderer::renderASCIIString(0, (pos++) * 16, "Direction:" + pack(Player::heading) + "Head:" + pack(Player::lookupdown) + "Jump speed:" + pack(Player::jump));

            {
                string tmp = "Stats:";
                if (Player::Flying) tmp += " Flying";
                if (Player::OnGround) tmp += " On_ground";
                if (Player::NearWall) tmp += " Near_wall";
                if (Player::inWater) tmp += " In_water";
                if (Player::CrossWall) tmp += " Cross_Wall";
                if (Player::Glide) tmp += " Gliding_enabled";
                if (Player::glidingNow) tmp += "Gliding";
                TextRenderer::renderASCIIString(0, (pos++) * 16, tmp);
            }

            TextRenderer::renderASCIIString(0, (pos++) * 16, "Energy:" + pack(Player::glidingEnergy));
            TextRenderer::renderASCIIString(0, (pos++) * 16, "Speed:" + pack(Player::glidingSpeed));

            {
                char tmp[50];
                sprintf(tmp, "Time:%02d:%02d:%02d(%d/%d)", gametime / (30 * 60), gametime % (30 * 60) / 30, gametime % 30 * 2, gametime, MaxGameTime);
                TextRenderer::renderASCIIString(0, (pos++) * 16, tmp);
            }

            TextRenderer::renderASCIIString(0, (pos++) * 16, "load:" + pack(World::chunks.size()) + " unload:" + pack(World::unloadedChunks) + " render:" + pack(WorldRenderer::RenderChunkList.size()) + " update:" + pack(World::updatedChunks));

            if (multiplayer)
            {
                MutexLock(Network::mutex);
                TextRenderer::renderASCIIString(0, (pos++) * 16, pack(Network::getRequestCount) + "/" + pack(NetworkRequestMax) + " network requests");
                MutexUnlock(Network::mutex);
            }

#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
            ss << c_getChunkPtrFromCPA << " CPA requests";
            TextRenderer::renderASCIIString(0, (pos++) * 16, ss.str());
            ss.str("");
            ss << c_getChunkPtrFromSearch << " search requests";
            TextRenderer::renderASCIIString(0, (pos++) * 16, ss.str());
            ss.str("");
            ss << c_getHeightFromHMap << " heightmap requests";
            TextRenderer::renderASCIIString(0, (pos++) * 16, ss.str());
            ss.str("");
            ss << c_getHeightFromWorldGen << " worldgen requests";
            TextRenderer::renderASCIIString(0, (pos++) * 16, ss.str());
            ss.str("");
#endif
        }
        else
        {
            TextRenderer::setFontColor(1.0f, 1.0f, 1.0f, 0.9f);
            std::stringstream ss;
            ss << "v" << version << "  Fps:" << fps;
            TextRenderer::renderString(10, 30, ss.str());
        }
        glFlush();
    }

    void DrawCloud(double px, double pz)
    {
        static double ltimer;
        static bool generated;
        static unsigned int cloudvb[128];
        static int vtxs[128];
        static float f;
        static int l;
        int i, j;

        if (ltimer == 0.0)
            ltimer = timer();
        f += (float)(timer() - ltimer) * 0.25f;
        ltimer = timer();
        if (f >= 1.0)
        {
            l += int(f);
            f -= int(f);
            l &= 127;
        }

        if (!generated)
        {
            generated = true;
            for (i = 0; i < 128; i++)
                for (j = 0; j < 128; j++)
                    World::cloud[i][j] = mersenne->get_s32_ranged(0, 2); // CHECK REQUIRED 真的是[0, 2) ?

            glGenBuffersARB(128, cloudvb);

            for (i = 0; i < 128; i++)
            {
                Renderer::Init(0, 0);
                for (j = 0; j < 128; j++)
                {
                    if (World::cloud[i][j] != 0)
                    {
                        Renderer::Vertex3d(j*CloudWidth, 128.0, 0.0);
                        Renderer::Vertex3d(j*CloudWidth, 128.0, CloudWidth);
                        Renderer::Vertex3d((j + 1)*CloudWidth, 128.0, CloudWidth);
                        Renderer::Vertex3d((j + 1)*CloudWidth, 128.0, 0.0);
                    }
                }
                Renderer::Flush(cloudvb[i], vtxs[i]);
            }
        }

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE);
        glColor4f(1.0, 1.0, 1.0, 0.5);
        for (int i = 0; i < 128; i++)
        {
            glPushMatrix();
            glTranslated(-64.0 * CloudWidth - px, 0.0, CloudWidth*(((l + i) & 127) + f) - 64.0 * CloudWidth - pz);
            Renderer::renderbuffer(cloudvb[i], vtxs[i], 0, 0);
            glPopMatrix();
        }
        //SetupNormalFog();
    }

    void RenderDestroy(float level, int x, int y, int z)
    {
        static float eps = 0.002f;
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        if (level < 100.0) glBindTexture(GL_TEXTURE_2D, DestroyImage[int(level / 10) + 1]);
        else glBindTexture(GL_TEXTURE_2D, DestroyImage[10]);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glEnd();
    }

    void DrawBagRow(int row, int itemid, int xbase, int ybase, int spac, float alpha)
    {
        //画出背包的一行
        for (int i = 0; i < 10; i++)
        {
            if (i == itemid) glBindTexture(GL_TEXTURE_2D, tex_select);
            else glBindTexture(GL_TEXTURE_2D, tex_unselect);
            glColor4f(1.0f, 1.0f, 1.0f, alpha);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 1.0);
            UIVertex(xbase + i * (32 + spac), ybase);
            glTexCoord2f(0.0, 0.0);
            UIVertex(xbase + i * (32 + spac) + 32, ybase);
            glTexCoord2f(1.0, 0.0);
            UIVertex(xbase + i * (32 + spac) + 32, ybase + 32);
            glTexCoord2f(1.0, 1.0);
            UIVertex(xbase + i * (32 + spac), ybase + 32);
            glEnd();
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            if (Player::inventory[row][i] != block(Blocks::AIR))
            {
                glBindTexture(GL_TEXTURE_2D, BlockTextures);
                double tcX = Textures::getTexcoordX(Player::inventory[row][i], 1);
                double tcY = Textures::getTexcoordY(Player::inventory[row][i], 1);
                glBegin(GL_QUADS);
                glTexCoord2d(tcX, tcY + 1 / 8.0);
                UIVertex(xbase + i * (32 + spac) + 2, ybase + 2);
                glTexCoord2d(tcX + 1 / 8.0, tcY + 1 / 8.0);
                UIVertex(xbase + i * (32 + spac) + 30, ybase + 2);
                glTexCoord2d(tcX + 1 / 8.0, tcY);
                UIVertex(xbase + i * (32 + spac) + 30, ybase + 30);
                glTexCoord2d(tcX, tcY);
                UIVertex(xbase + i * (32 + spac) + 2, ybase + 30);
                glEnd();
                std::stringstream ss;
                ss << (int)Player::inventoryAmount[row][i];
                TextRenderer::renderString(xbase + i * (32 + spac), ybase, ss.str());
            }
        }
    }

    void DrawBag()
    {
        //背包界面与更新
        static int si, sj, sf;
        int csi = -1, csj = -1;
        int leftp = static_cast<int>((windowwidth / stretch - 392) / 2);
        int upp = getStretchedIntWindowHeight() - 152 - 16;
        static int mousew, mouseb, mousebl;
        static block indexselected = block(Blocks::AIR);
        static short Amountselected = 0;
        double curtime = timer();
        double TimeDelta = curtime - bagAnimTimer;
        float bagAnim = (float)(1.0 - pow(0.9, TimeDelta*60.0) + pow(0.9, bagAnimDuration*60.0) / bagAnimDuration * TimeDelta);

        if (bagOpened)
        {

            glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mousew = mw;
            mouseb = mb;
            glDepthFunc(GL_ALWAYS);
            glDisable(GL_CULL_FACE);
            glDisable(GL_TEXTURE_2D);

            if (curtime - bagAnimTimer > bagAnimDuration) glColor4f(0.2f, 0.2f, 0.2f, 0.6f);
            else glColor4f(0.2f, 0.2f, 0.2f, 0.6f*bagAnim);
            glBegin(GL_QUADS);
            UIVertex(0, 0);
            UIVertex(getStretchedIntWindowWidth(), 0);
            UIVertex(getStretchedIntWindowWidth(), getStretchedIntWindowHeight());
            UIVertex(0, getStretchedIntWindowHeight());
            glEnd();

            glEnable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            sf = 0;

            if (curtime - bagAnimTimer > bagAnimDuration)
            {
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        if (mx >= j * 40 + leftp && mx <= j * 40 + 32 + leftp &&
                                my >= i * 40 + upp && my <= i * 40 + 32 + upp)
                        {
                            csi = si = i;
                            csj = sj = j;
                            sf = 1;
                            if (mousebl == 0 && mouseb == 1 && indexselected == Player::inventory[i][j])
                            {
                                if (Player::inventoryAmount[i][j] + Amountselected <= 255)
                                {
                                    Player::inventoryAmount[i][j] += Amountselected;
                                    Amountselected = 0;
                                }
                                else
                                {
                                    Amountselected = Player::inventoryAmount[i][j] + Amountselected - 255;
                                    Player::inventoryAmount[i][j] = 255;
                                }
                            }
                            if (mousebl == 0 && mouseb == 1 && indexselected != Player::inventory[i][j])
                            {
                                std::swap(Amountselected, Player::inventoryAmount[i][j]);
                                std::swap(indexselected, Player::inventory[i][j]);
                            }
                            if (mousebl == 0 && mouseb == 2 && indexselected == Player::inventory[i][j] && Player::inventoryAmount[i][j] < 255)
                            {
                                Amountselected--;
                                Player::inventoryAmount[i][j]++;
                            }
                            if (mousebl == 0 && mouseb == 2 && Player::inventory[i][j] == block(Blocks::AIR))
                            {
                                Amountselected--;
                                Player::inventoryAmount[i][j] = 1;
                                Player::inventory[i][j] = indexselected;
                            }

                            if (Amountselected == 0) indexselected = block(Blocks::AIR);
                            if (indexselected == block(Blocks::AIR)) Amountselected = 0;
                            if (Player::inventoryAmount[i][j] == 0) Player::inventory[i][j] = block(Blocks::AIR);
                            if (Player::inventory[i][j] == block(Blocks::AIR)) Player::inventoryAmount[i][j] = 0;
                        }
                    }
                    DrawBagRow(i, (csi == i ? csj : -1), static_cast<int>((windowwidth / stretch - 392) / 2), getStretchedIntWindowHeight() - 152 - 16 + i * 40, 8, 1.0f);
                }
            }
            if (indexselected != block(Blocks::AIR))
            {
                glBindTexture(GL_TEXTURE_2D, BlockTextures);
                double tcX = Textures::getTexcoordX(indexselected, 1);
                double tcY = Textures::getTexcoordY(indexselected, 1);
                glBegin(GL_QUADS);
                glTexCoord2d(tcX, tcY + 1 / 8.0);
                UIVertex(mx - 16, my - 16);
                glTexCoord2d(tcX + 1 / 8.0, tcY + 1 / 8.0);
                UIVertex(mx + 16, my - 16);
                glTexCoord2d(tcX + 1 / 8.0, tcY);
                UIVertex(mx + 16, my + 16);
                glTexCoord2d(tcX, tcY);
                UIVertex(mx - 16, my + 16);
                glEnd();
                std::stringstream ss;
                ss << Amountselected;
                TextRenderer::renderString((int)mx - 16, (int)my - 16, ss.str());
            }
            if (Player::inventory[si][sj] != 0 && sf == 1)
            {
                glColor4f(1.0, 1.0, 0.0, 1.0);
                TextRenderer::renderString((int)mx, (int)my - 16, BlockInfo(Player::inventory[si][sj]).getBlockName());
            }

            int xbase = 0, ybase = 0, spac = 0;
            float alpha = 0.5f + 0.5f*bagAnim;
            if (curtime - bagAnimTimer <= bagAnimDuration)
            {
                xbase = (int)round(((windowwidth / stretch - 392) / 2)*bagAnim);
                ybase = (int)round((windowheight / stretch - 152 - 16 + 120 - (windowheight / stretch - 32))*bagAnim + (windowheight / stretch - 32));
                spac = (int)round(8 * bagAnim);
                DrawBagRow(3, -1, xbase, ybase, spac, alpha);
                xbase = (int)round(((windowwidth / stretch - 392) / 2 - windowwidth / stretch)*bagAnim + windowwidth / stretch);
                ybase = (int)round((windowheight / stretch - 152 - 16 - (windowheight / stretch - 32))*bagAnim + (windowheight / stretch - 32));
                for (int i = 0; i < 3; i++)
                {
                    glColor4f(1.0f, 1.0f, 1.0f, bagAnim);
                    DrawBagRow(i, -1, xbase, ybase + i * 40, spac, alpha);
                }
            }

            glEnable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            mousebl = mouseb;
        }
        else
        {

            glEnable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);
            if (curtime - bagAnimTimer <= bagAnimDuration)
            {
                glDisable(GL_TEXTURE_2D);
                glColor4f(0.2f, 0.2f, 0.2f, 0.6f - 0.6f*bagAnim);
                glBegin(GL_QUADS);
                glVertex2i(0, 0);
                glVertex2i(windowwidth, 0);
                glVertex2i(windowwidth, windowheight);
                glVertex2i(0, windowheight);
                glEnd();
                glEnable(GL_TEXTURE_2D);
                int xbase = 0, ybase = 0, spac = 0;
                float alpha = 1.0f - 0.5f*bagAnim;
                xbase = (int)round(((windowwidth / stretch - 392) / 2) - ((windowwidth / stretch - 392) / 2)*bagAnim);
                ybase = (int)round((windowheight / stretch - 152 - 16 + 120 - (windowheight / stretch - 32)) - (windowheight / stretch - 152 - 16 + 120 - (windowheight - 32))*bagAnim + (windowheight / stretch - 32));
                spac = (int)round(8 - 8 * bagAnim);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                DrawBagRow(3, Player::indexInHand, xbase, ybase, spac, alpha);
                xbase = (int)round(((windowwidth / stretch - 392) / 2 - windowwidth / stretch) - ((windowwidth / stretch - 392) / 2 - windowwidth / stretch)*bagAnim + windowwidth / stretch);
                ybase = (int)round((windowheight / stretch - 152 - 16 - (windowheight / stretch - 32)) - (windowheight / stretch - 152 - 16 - (windowheight / stretch - 32))*bagAnim + (windowheight / stretch - 32));
                for (int i = 0; i < 3; i++)
                {
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f - bagAnim);
                    DrawBagRow(i, -1, xbase, ybase + i * 40, spac, alpha);
                }
            }
            else DrawBagRow(3, Player::indexInHand, 0, getStretchedIntWindowHeight() - 32, 0, 0.5f);
        }
        glFlush();
    }

    void SaveScreenshot(int x, int y, int w, int h, string filename)
    {
        int bufw = ((w >> 2) << 2) + (w & 3 ? 4 : 0), bufh = ((h >> 2) << 2) + (h & 3 ? 4 : 0);
        Textures::TEXTURE_RGB scrBuffer(bufw, bufh);
        glReadPixels(x, y, bufw, bufh, GL_RGB, GL_UNSIGNED_BYTE, scrBuffer.buffer.get());
        scrBuffer.Save(filename);
    }

    void CreateThumbnail()
    {
        SaveScreenshot(0, 0, windowwidth, windowheight, "Worlds/" + World::WorldName + "/Thumbnail.bmp");
    }

    void RegisterCommands()
    {
#ifdef NEWORLD_TARGET_WINDOWS
        register_command(L"/give", [](int argc, const wchar_t** argv) -> int
        {
            if (argc != 3) return 0;
            item itemid;
            itemid.ID = extract<unsigned short>(argv[1]);
            Player::AddItem(itemid, extract<short>(argv[2]));
            return 1;
        });
        register_command(L"/tp", [](int argc, const wchar_t** argv) -> int
        {
            if (argc != 4) return 0;
            Player::xpos = extract<double>(argv[1]);
            Player::ypos = extract<double>(argv[2]);
            Player::zpos = extract<double>(argv[3]);
            return 1;
        });
        register_command(L"/suicide", [](int, const wchar_t**) -> int
        {
            Player::spawn();
            return 1;
        });
        register_command(L"/setblock", [](int argc, const wchar_t** argv) -> int
        {
            if (argc != 5) return 0;
            block b;
            b.ID = extract<unsigned short>(argv[4]);
            World::setblock(extract<int>(argv[1]), extract<int>(argv[2]), extract<int>(argv[3]), b);
            return 1;
        });
        register_command(L"/tree", [](int argc, const wchar_t** argv) -> int
        {
            if (argc != 4) return 0;
            World::buildtree(extract<int>(argv[1]), extract<int>(argv[2]), extract<int>(argv[3]));
            return 1;
        });
        register_command(L"/explode", [](int argc, const wchar_t** argv) -> int
        {
            if (argc != 5) return 0;
            World::explode(extract<int>(argv[1]), extract<int>(argv[2]), extract<int>(argv[3]), extract<int>(argv[4]));
            return 1;
        });
        register_command(L"/gamemode", [](int argc, const wchar_t** argv) -> int
        {
            if (argc != 2) return 0;
            Player::changeGameMode(extract<int>(argv[1]));
            return 1;
        });
        register_command(L"/kit", [](int argc, const wchar_t** argv) -> int
        {
            for (int i = 0; i <= 1; i++)
                for (int j = 0; j <= i ? 7 : 9; j++)
                {
                    Player::inventory[i][j] = i * 10 + j + 1;
                    Player::inventoryAmount[i][j] = 255;
                }
            return 1;
        });
        register_command(L"/time", [](int argc, const wchar_t** argv) -> int
        {
            if (argc != 2) return 0;
            int time = extract<int>(argv[1]);
            if (time<0 || time>MaxGameTime) return 0;
            gametime = time;
            return 1;
        });
#endif
    }

    bool ExecuteCommand(const vector<string>& command)
    {
#ifdef NEWORLD_TARGET_WINDOWS
        vector<std::wstring> tmp;
        for (int i = 0; i < (int)command.size(); i++)
            tmp.push_back(StringToWString(command[i]));
        if (commands.count(tmp[0]))
            return commands[tmp[0]].execute(tmp) != 0;
#endif
        return false;
    }

    void onLoad()
    {
        Background = nullptr;

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_TEXTURE_2D);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();

        Mutex = MutexCreate();
        MutexLock(Mutex);
        updateThread = ThreadCreate(&UpdateThreadFunc, NULL);
        if (multiplayer)
        {
            Player::name = "";
            Player::onlineID = mersenne->get_u16();
            Network::init(serverip, port);
        }
        //初始化游戏状态
        if (LoadGame())
            Player::init(Player::xpos, Player::ypos, Player::zpos);
        else
            Player::spawn();
        World::Init();
        static bool registered = false;
        if (!registered)
        {
            RegisterCommands();
            registered = true;
        }

        GUIrenderswitch = true;
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        SetupNormalFog();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();

        //这才是游戏开始!
        glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mxl = mx;
        myl = my;
        updateThreadRun = true;
        fctime = uctime = lastupdate = timer();
    }

    void onUpdate()
    {

        MutexUnlock(Mutex);
        MutexLock(Mutex);

        if ((timer() - uctime) >= 1.0)
        {
            uctime = timer();
            ups = upsc;
            upsc = 0;
        }

        Grender();

        if (glfwGetKey(MainWindow, GLFW_KEY_ESCAPE) == 1)
        {
            updateThreadPaused = true;
            CreateThumbnail();
            GUI::clearTransition();
            GUI::PushPage(new Menus::GameMenu);
        }
    }

    void onLeave()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TextRenderer::setFontColor(1.0, 1.0, 1.0, 1.0);
        TextRenderer::renderString(0, 0, "Saving world...");
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
        updateThreadRun = false;
        MutexUnlock(Mutex);
        ThreadWait(updateThread);
        ThreadDestroy(updateThread);
        MutexDestroy(Mutex);
        SaveGame();
        World::destroyAllChunks();
        if (World::vbuffersShouldDelete.size() > 0)
        {
            glDeleteBuffersARB(World::vbuffersShouldDelete.size(), World::vbuffersShouldDelete.data());
            World::vbuffersShouldDelete.clear();
        }
        if (multiplayer) Network::cleanUp();
        chatMessages.clear();
        GUI::BackToMain();
    }
};
GameDView* Game = nullptr;
void GameView()
{
    GUI::PushPage(Game = (new GameDView));
}

ThreadFunc UpdateThreadFunc(void*)
{
    Game->GameThreadloop();
    return 0;
}
