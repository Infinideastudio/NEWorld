#pragma once

#include "CommandHandler.h"
#include <al.h>
#include "AudioSystem.h"


class Game : public CommandHandler {
    int oldselx{};
    int oldsely{};
    int oldselz{};
    Brightness selbr{};
protected:
    bool DebugHitbox{};
    bool DebugMergeFace{};
    bool DebugMode{};
    bool DebugShadow{};
    bool GUIrenderswitch{};
    std::vector<std::string> chatMessages;
    bool chatmode = false;
    std::string chatword;
    bool sel{};
    Block selb{};
    float seldes{};
    int selx{};
    int sely{};
    int selz{};
public:
    void PlayerGlideEnergyDamper() {
        auto &E = Player::glidingEnergy;
        const auto oldh = Player::Pos.Y + Player::height + Player::heightExt + Player::ya;
        auto h = oldh;
        if (E - Player::glidingMinimumSpeed < h * g) {  //小于最小速度
            h = (E - Player::glidingMinimumSpeed) / g;
        }
        Player::glidingSpeed = sqrt(2 * (E - g * h));
        E -= EDrop;
        Player::ya += h - oldh;
    }

    void PlayerActiveItemSelect() {
        //切换方块
        if (isPressed(GLFW_KEY_Z) && Player::indexInHand > 0) Player::indexInHand--;
        if (isPressed(GLFW_KEY_X) && Player::indexInHand < 9) Player::indexInHand++;
        if (static_cast<int>(Player::indexInHand) + (mwl - mw) < 0)Player::indexInHand = 0;
        else if (static_cast<int>(Player::indexInHand) + (mwl - mw) > 9)Player::indexInHand = 9;
        else Player::indexInHand += static_cast<char>(mwl - mw);
        mwl = mw;
    }

    void updategame() {
        //Time_updategame_ = timer();
        static double Wprstm;
        static bool WP;
        //bool chunkupdated = false;

        //用于音效更新
        auto BlockClick = false;
        ALfloat BlockPos[3];

        Player::BlockInHand = Player::inventory[3][Player::indexInHand];
        //生命值相关
        if (Player::health > 0 || Player::gamemode == Player::Creative) {
            if (Player::Pos.Y < -100) Player::health -= ((-100) - Player::Pos.Y) / 100;
            if (Player::health < Player::healthMax) Player::health += Player::healSpeed;
            if (Player::health > Player::healthMax) Player::health = Player::healthMax;
        } else Player::spawn();

        //时间
        gametime++;
        if (glfwGetKey(MainWindow, GLFW_KEY_F8)) gametime += 30;
        if (gametime > gameTimeMax) gametime = 0;

        World::unloadedChunks = 0;
        World::rebuiltChunks = 0;
        World::updatedChunks = 0;

        //cpArray move
        World::cpArray.MoveTo(
                {Player::cxt - viewdistance - 2, Player::cyt - viewdistance - 2, Player::czt - viewdistance - 2});

        //HeightMap move
        if (World::HMap.originX != (Player::cxt - viewdistance - 2) * 16 ||
            World::HMap.originZ != (Player::czt - viewdistance - 2) * 16) {
            World::HMap.moveTo((Player::cxt - viewdistance - 2) * 16, (Player::czt - viewdistance - 2) * 16);
        }

        if (FirstUpdateThisFrame) {
            ChunkLoadUnload();
        }

        //加载动画
        for (auto cp : World::chunks) {
            if (cp->loadAnim <= 0.3f) cp->loadAnim = 0.0f;
            else cp->loadAnim *= 0.6f;
        }

        //随机状态更新
        RandomTick();

        auto lx = Player::Pos.X;
        auto ly = Player::Pos.Y + Player::height + Player::heightExt;
        auto lz = Player::Pos.Z;

        sel = false;
        selx = sely = selz = selb = selbr = 0;

        if (!bagOpened) {
            BlockClick = PlayerInteract(BlockPos, lx, ly, lz);
            Player::IntPos = Int3(Player::Pos, RoundInt);

            auto curtime = timer();

            //转头！你治好了我多年的颈椎病！
            if (mx != mxl) Player::xlookspeed -= (mx - mxl) * mousemove;
            if (my != myl) Player::ylookspeed += (my - myl) * mousemove;
            if (glfwGetKey(MainWindow, GLFW_KEY_RIGHT) == 1)
                Player::xlookspeed -= mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_LEFT) == 1)
                Player::xlookspeed += mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_UP) == 1)
                Player::ylookspeed -= mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_DOWN) == 1)
                Player::ylookspeed += mousemove * 16 * (curtime - lastframe) * 30.0;

            //限制角度，别把头转掉下来了 ←_←
            if (Player::lookupdown + Player::ylookspeed < -90.0) Player::ylookspeed = -90.0 - Player::lookupdown;
            if (Player::lookupdown + Player::ylookspeed > 90.0) Player::ylookspeed = 90.0 - Player::lookupdown;

            //更新方向
            Player::heading += Player::xlookspeed;
            Player::heading = fmod(Player::heading, 360.0);
            Player::lookupdown += Player::ylookspeed;
            Player::xlookspeed = Player::ylookspeed = 0.0;

            if (!chatmode) {
                WP = ProcessPlayerNavigate(Wprstm, WP);
                PlayerActiveItemSelect();
                //起跳！
                PlayerStartJump(Wprstm);

                if ((glfwGetKey(MainWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                    glfwGetKey(MainWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) && !Player::glidingNow) {
                    if (Player::CrossWall || Player::Flying) Player::ya -= walkspeed / 2;
                    Wprstm = 0.0;
                }

                if (glfwGetKey(MainWindow, GLFW_KEY_K) && Player::Glide && !Player::OnGround && !Player::glidingNow) {
                    const auto h = Player::Pos.Y + Player::height + Player::heightExt;
                    Player::glidingEnergy = g * h;
                    Player::glidingSpeed = 0;
                    Player::glidingNow = true;
                }
                HotkeySettingsToggle();

            }

            HandleChatAndCommand();
        }

        inputstr = "";

        if (isPressed(GLFW_KEY_E) && GUIrenderswitch && !chatmode) {
            bagOpened = !bagOpened;
            bagAnimTimer = timer();
            if (!bagOpened) {
                glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                shouldGetThumbnail = true;
                Player::xlookspeed = Player::ylookspeed = 0.0;
            }
        }

        if (!bagOpened && !chatmode) {
            if (isPressed(GLFW_KEY_L))World::saveAllChunks();
        }

        //跳跃
        ProcessPlayerJump();

        if (Player::glidingNow) {
            PlayerGlideEnergyDamper();
        }

        AudioUpdate(WP, BlockClick, BlockPos);


        mbp = mb;
        FirstFrameThisUpdate = true;
        Particles::updateall();

        Player::IntPos = Int3(Player::Pos, RoundInt);
        Player::updatePosition();
        Player::PosOld = Player::Pos;
        Player::IntPosOld = Int3(Player::Pos, RoundInt);

        //	Time_updategame += timer() - Time_updategame;
    }

    void AudioUpdate(bool WP, bool blockClick, ALfloat *blockPos) const {//音效更新
        auto Run = 0;
        if (WP)Run = Player::Running ? 2 : 1;
        ALfloat PlayerPos[3];
        PlayerPos[0] = Player::Pos.X;
        PlayerPos[1] = Player::Pos.Y;
        PlayerPos[2] = Player::Pos.Z;
        const auto Fall = Player::OnGround
                          && (!Player::inWater)
                          && (Player::jump == 0);
        //更新声速
        AudioSystem::SpeedOfSound = Player::inWater ? AudioSystem::Water_SpeedOfSound : AudioSystem::Air_SpeedOfSound;
        //更新环境
        if (Player::inWater) {
            //EFX::EAXprop = UnderWater;
        } else {
            if (Player::OnGround) {
                //EFX::EAXprop = Plain;
            } else {
                //EFX::EAXprop = Generic;
            }
        }
        //EFX::UpdateEAXprop();
        AudioSystem::Update(PlayerPos, Fall, blockClick, blockPos, Run, Player::inWater);
    }

    void ChunkLoadUnload() const {
        World::sortChunkLoadUnloadList(Player::IntPos);
        for (const auto&[_, chunk] : World::ChunkUnloadList) {
            const auto c = chunk->GetPosition();
            World::DeleteChunk(c);
        }
        for (const auto&[_, pos]: World::ChunkLoadList) {
            auto c = World::AddChunk(pos);
            c->Load(false);
            if (c->Empty) {
                World::DeleteChunk(pos);
                World::cpArray.Set(pos, World::EmptyChunkPtr);
            }
        }
    }

    void PlayerStartJump(double &Wprstm) const {
        if (isPressed(GLFW_KEY_SPACE)) {
            if (!Player::inWater) {
                if ((Player::OnGround || Player::AirJumps < MaxAirJumps) && !Player::Flying &&
                    !Player::CrossWall) {
                    if (!Player::OnGround) {
                        Player::jump = 0.3;
                        Player::AirJumps++;
                    } else {
                        Player::jump = 0.25;
                        Player::OnGround = false;
                    }
                }
                if (Player::Flying || Player::CrossWall) {
                    Player::ya += walkspeed / 2;
                    isPressed(GLFW_KEY_SPACE, true);
                }
                Wprstm = 0.0;
            } else {
                Player::ya = walkspeed;
                isPressed(GLFW_KEY_SPACE, true);
            }
        }
    }

    void HandleChatAndCommand() {
        if (isPressed(GLFW_KEY_ENTER) == GLFW_PRESS) {
            chatmode = !chatmode;
            if (!chatword.empty()) { //指令的执行，或发出聊天文本
                if (chatword.substr(0, 1) == "/") { //指令
                    const auto command = split(chatword, " ");
                    if (!doCommand(command)) { //执行失败
                        DebugWarning("Fail to execute the command: " + chatword);
                        chatMessages.push_back("Fail to execute the command: " + chatword);
                    }
                } else {
                    chatMessages.push_back(chatword);
                }
            }
            chatword = "";
        }
        if (chatmode) {
            if (isPressed(GLFW_KEY_BACKSPACE) && chatword.length() > 0) {
                const int n = chatword[chatword.length() - 1];
                if (n > 0 && n <= 127)
                    chatword = chatword.substr(0, chatword.length() - 1);
                else
                    chatword = chatword.substr(0, chatword.length() - 2);
            } else {
                chatword += inputstr;
            }
            //自动补全
            if (isPressed(GLFW_KEY_TAB) && chatmode && !chatword.empty() && chatword.substr(0, 1) == "/") {
                for (unsigned int i = 0; i != commands.size(); i++) {
                    if (beginWith(commands[i].identifier, chatword)) {
                        chatword = commands[i].identifier;
                    }
                }
            }
        }
    }

    void HotkeySettingsToggle() {//各种设置切换
        if (isPressed(GLFW_KEY_F1)) {
            Player::changeGameMode(Player::gamemode == Player::Creative ?
                                   Player::Survival : Player::Creative);
        }
        if (isPressed(GLFW_KEY_F2)) shouldGetScreenshot = true;
        if (isPressed(GLFW_KEY_F3)) DebugMode = !DebugMode;
        if (isPressed(GLFW_KEY_F4)) Player::CrossWall = !Player::CrossWall;
        if (isPressed(GLFW_KEY_H) && glfwGetKey(MainWindow, GLFW_KEY_F3) == GLFW_PRESS) {
            DebugHitbox = !DebugHitbox;
            DebugMode = true;
        }
        if (Renderer::AdvancedRender) {
            if (isPressed(GLFW_KEY_M) && glfwGetKey(MainWindow, GLFW_KEY_F3) == GLFW_PRESS) {
                DebugShadow = !DebugShadow;
                DebugMode = true;
            }
        } else DebugShadow = false;
        if (isPressed(GLFW_KEY_G) && glfwGetKey(MainWindow, GLFW_KEY_F3) == GLFW_PRESS) {
            DebugMergeFace = !DebugMergeFace;
            DebugMode = true;
        }
        if (isPressed(GLFW_KEY_F4 && Player::gamemode == Player::Creative))
            Player::CrossWall = !Player::CrossWall;
        if (isPressed(GLFW_KEY_F5)) GUIrenderswitch = !GUIrenderswitch;
        if (isPressed(GLFW_KEY_F6)) Player::Glide = !Player::Glide;
        if (isPressed(GLFW_KEY_F7)) Player::spawn();
        if (isPressed(GLFW_KEY_SLASH)) chatmode = true; //斜杠将会在下面的if(chatmode)里添加
    }

    bool ProcessPlayerNavigate(double &Wprstm, bool WP) const {//移动！(生命在于运动)
        if (glfwGetKey(MainWindow, GLFW_KEY_W) || Player::glidingNow) {
            if (!WP) {
                if (Wprstm == 0.0) {
                    Wprstm = timer();
                } else {
                    if (timer() - Wprstm <= 0.5) {
                        Player::Running = true;
                        Wprstm = 0.0;
                    } else Wprstm = timer();
                }
            }
            if (Wprstm != 0.0 && timer() - Wprstm > 0.5) Wprstm = 0.0;
            WP = true;
            if (!Player::glidingNow) {
                Player::xa += -sin(Player::heading * M_PI / 180.0) * Player::speed;
                Player::za += -cos(Player::heading * M_PI / 180.0) * Player::speed;
            } else {
                Player::xa = sin(M_PI / 180 * (Player::heading - 180)) *
                             sin(M_PI / 180 * (Player::lookupdown + 90)) * Player::glidingSpeed * speedCast;
                Player::ya = cos(M_PI / 180 * (Player::lookupdown + 90)) * Player::glidingSpeed * speedCast;
                Player::za = cos(M_PI / 180 * (Player::heading - 180)) *
                             sin(M_PI / 180 * (Player::lookupdown + 90)) * Player::glidingSpeed * speedCast;
                if (Player::ya < 0) Player::ya *= 2;
            }
        } else {
            Player::Running = false;
            WP = false;
        }
        if (Player::Running)Player::speed = runspeed;
        else Player::speed = walkspeed;

        if (glfwGetKey(MainWindow, GLFW_KEY_S) == GLFW_PRESS && !Player::glidingNow) {
            Player::xa += sin(Player::heading * M_PI / 180.0) * Player::speed;
            Player::za += cos(Player::heading * M_PI / 180.0) * Player::speed;
            Wprstm = 0.0;
        }

        if (glfwGetKey(MainWindow, GLFW_KEY_A) == GLFW_PRESS && !Player::glidingNow) {
            Player::xa += sin((Player::heading - 90) * M_PI / 180.0) * Player::speed;
            Player::za += cos((Player::heading - 90) * M_PI / 180.0) * Player::speed;
            Wprstm = 0.0;
        }

        if (glfwGetKey(MainWindow, GLFW_KEY_D) == GLFW_PRESS && !Player::glidingNow) {
            Player::xa += -sin((Player::heading - 90) * M_PI / 180.0) * Player::speed;
            Player::za += -cos((Player::heading - 90) * M_PI / 180.0) * Player::speed;
            Wprstm = 0.0;
        }

        if (!Player::Flying && !Player::CrossWall) {
            const auto horizontalSpeed = sqrt(Player::xa * Player::xa + Player::za * Player::za);
            if (horizontalSpeed > Player::speed && !Player::glidingNow) {
                Player::xa *= Player::speed / horizontalSpeed;
                Player::za *= Player::speed / horizontalSpeed;
            }
        } else {
            if (glfwGetKey(MainWindow, GLFW_KEY_R) == GLFW_PRESS && !Player::glidingNow) {
                if (glfwGetKey(MainWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                    Player::xa = -sin(Player::heading * M_PI / 180.0) * runspeed * 10;
                    Player::za = -cos(Player::heading * M_PI / 180.0) * runspeed * 10;
                } else {
                    Player::xa = sin(M_PI / 180 * (Player::heading - 180)) *
                                 sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                    Player::ya = cos(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                    Player::za = cos(M_PI / 180 * (Player::heading - 180)) *
                                 sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                }
            }

            if (glfwGetKey(MainWindow, GLFW_KEY_F) == GLFW_PRESS && !Player::glidingNow) {
                if (glfwGetKey(MainWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                    Player::xa = sin(Player::heading * M_PI / 180.0) * runspeed * 10;
                    Player::za = cos(Player::heading * M_PI / 180.0) * runspeed * 10;
                } else {
                    Player::xa = -sin(M_PI / 180 * (Player::heading - 180)) *
                                 sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                    Player::ya = -cos(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                    Player::za = -cos(M_PI / 180 * (Player::heading - 180)) *
                                 sin(M_PI / 180 * (Player::lookupdown + 90)) * runspeed * 20;
                }
            }
        }
        return WP;
    }

    void ProcessPlayerJump() const {
        if (!Player::glidingNow) {
            if (!Player::inWater) {
                if (!Player::Flying && !Player::CrossWall) {
                    Player::ya = -0.001;
                    if (Player::OnGround) {
                        Player::jump = 0.0;
                        Player::AirJumps = 0;
                        isPressed(GLFW_KEY_SPACE, true);
                    } else {
                        //自由落体计算
                        Player::jump -= 0.025;
                        Player::ya = Player::jump + 0.5 * 0.6 / 900.0;
                    }
                } else {
                    Player::jump = 0.0;
                    Player::AirJumps = 0;
                }
            } else {
                Player::jump = 0.0;
                Player::AirJumps = MaxAirJumps;
                isPressed(GLFW_KEY_SPACE, true);
                if (Player::ya <= 0.001 && !Player::Flying && !Player::CrossWall) {
                    Player::ya = -0.001;
                    if (!Player::OnGround) Player::ya -= 0.1;
                }
            }
        }
    }

    bool PlayerInteract(ALfloat *BlockPos, double lx, double ly, double lz) {
        bool blockClick{false};//从玩家位置发射一条线段
        for (auto i = 0; i < selectPrecision * selectDistance; i++) {
            const auto lxl = lx;
            const auto lyl = ly;
            const auto lzl = lz;

            //线段延伸
            lx += sin(M_PI / 180 * (Player::heading - 180)) * sin(M_PI / 180 * (Player::lookupdown + 90)) /
                  static_cast<double>(selectPrecision);
            ly += cos(M_PI / 180 * (Player::lookupdown + 90)) / static_cast<double>(selectPrecision);
            lz += cos(M_PI / 180 * (Player::heading - 180)) * sin(M_PI / 180 * (Player::lookupdown + 90)) /
                  static_cast<double>(selectPrecision);

            //碰到方块
            if (BlockInfo(World::GetBlock({RoundInt(lx), RoundInt(ly), RoundInt(lz)})).isSolid()) {
                const auto x = RoundInt(lx);
                const auto y = RoundInt(ly);
                const auto z = RoundInt(lz);
                const auto xl = RoundInt(lxl);
                const auto yl = RoundInt(lyl);
                const auto zl = RoundInt(lzl);

                selx = x;
                sely = y;
                selz = z;
                sel = true;

                //找方块所在区块及位置

                selbr = World::getbrightness(xl, yl, zl);
                selb = World::GetBlock({(x), (y), (z)});
                if (mb == 1 || glfwGetKey(MainWindow, GLFW_KEY_ENTER) == GLFW_PRESS) {
                    Particles::throwParticle(selb,
                                             float(x + rnd() - 0.5f), float(y + rnd() - 0.2f),
                                             float(z + rnd() - 0.5f),
                                             float(rnd() * 0.2f - 0.1f), float(rnd() * 0.2f - 0.1f),
                                             float(rnd() * 0.2f - 0.1f),
                                             float(rnd() * 0.01f + 0.02f), int(rnd() * 30) + 30);

                    if (selx != oldselx || sely != oldsely || selz != oldselz) seldes = 0.0;
                    else {
                        float factor {};
                        if (Player::inventory[3][Player::indexInHand] == STICK)factor = 4;
                        else
                            factor = 30.0 /
                                     (BlockInfo(Player::inventory[3][Player::indexInHand]).getHardness() + 0.1);
                        if (factor < 1.0)factor = 1.0;
                        if (factor > 1.7)factor = 1.7;
                        seldes += BlockInfo(selb).getHardness() *
                                  ((Player::gamemode == Player::Creative) ? 10.0f : 0.3f) * factor;
                        blockClick = true;
                        BlockPos[0] = x;
                        BlockPos[1] = y;
                        BlockPos[2] = z;

                    }

                    if (seldes >= 100.0) {
                        for (auto j = 1; j <= 25; j++) {
                            Particles::throwParticle(selb,
                                                     float(x + rnd() - 0.5f), float(y + rnd() - 0.2f),
                                                     float(z + rnd() - 0.5f),
                                                     float(rnd() * 0.2f - 0.1f), float(rnd() * 0.2f - 0.1f),
                                                     float(rnd() * 0.2f - 0.1f),
                                                     float(rnd() * 0.02 + 0.03), int(rnd() * 60) + 30);
                        }
                        World::PickBlock({(x), (y), (z)});
                        blockClick = true;
                        BlockPos[0] = x;
                        BlockPos[1] = y;
                        BlockPos[2] = z;
                    }
                }
                if (((mb == 2 && mbp == 0) || (!chatmode && isPressed(GLFW_KEY_TAB)))) { //鼠标右键
                    if (Player::inventoryAmount[3][Player::indexInHand] > 0 &&
                        isBlock(Player::inventory[3][Player::indexInHand])) {
                        //放置方块
                        if (Player::putBlock(xl, yl, zl, Player::BlockInHand)) {
                            Player::inventoryAmount[3][Player::indexInHand]--;
                            if (Player::inventoryAmount[3][Player::indexInHand] == 0)
                                Player::inventory[3][Player::indexInHand] = Blocks::ENV;

                            blockClick = true;
                            BlockPos[0] = x;
                            BlockPos[1] = y;
                            BlockPos[2] = z;
                        }
                    } else {
                        //使用物品
                        if (Player::inventory[3][Player::indexInHand] == APPLE) {
                            Player::inventoryAmount[3][Player::indexInHand]--;
                            if (Player::inventoryAmount[3][Player::indexInHand] == 0)
                                Player::inventory[3][Player::indexInHand] = Blocks::ENV;
                            Player::health = Player::healthMax;
                        }
                    }
                }
                break;
            }
        }

        if (selx != oldselx || sely != oldsely || selz != oldselz ||
            (mb == 0 && glfwGetKey(MainWindow, GLFW_KEY_ENTER) != GLFW_PRESS))
            seldes = 0.0;
        oldselx = selx;
        oldsely = sely;
        oldselz = selz;
        return blockClick;
    }

    void RandomTick() const {
        for (auto &chunk : World::chunks) {
            const auto cPos = chunk->GetPosition();
            const auto bPos = Int3{std::min(15, int(rnd() * 16)), std::min(15, int(rnd() * 16)), std::min(15, int(rnd() * 16))};
            const auto gPos = (cPos << World::ChunkEdgeSizeLog2) + bPos;
            const auto block = chunk->GetBlock(bPos);
            if (block != Blocks::ENV) {
                BlockInfo(block).OnRandomTick(gPos, block);
            }
        }
    }

    static bool isPressed(int key, bool setFalse = false) {
        static bool keyPressed[GLFW_KEY_LAST + 1];
        if (setFalse) {
            keyPressed[key] = false;
            return true;
        }
        if (key > GLFW_KEY_LAST || key <= 0) return false;
        if (!glfwGetKey(MainWindow, key)) keyPressed[key] = false;
        if (!keyPressed[key] && glfwGetKey(MainWindow, key)) {
            keyPressed[key] = true;
            return true;
        }
        return false;
    }

    static void saveGame() {
        World::saveAllChunks();
        if (!Player::save(World::worldname)) {
#ifdef NEWORLD_CONSOLE_OUTPUT
            DebugWarning("Failed saving player info!");
#endif
        }
    }

    static bool loadGame() {
        if (!Player::load(World::worldname)) {
#ifdef NEWORLD_CONSOLE_OUTPUT
            DebugWarning("Failed loading player info!");
#endif
            return false;
        }
        return true;
    }
};
