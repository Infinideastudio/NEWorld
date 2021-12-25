#include "World.h"
#include "Particles.h"
#include <cmath>
#include <algorithm>
#include "System/FileSystem.h"
#include "TerrainGen/Generate.h"
#include "Renderer/World/WorldRenderer.h"

namespace World {
    std::string worldname;
    Brightness skylight = 15;         //Sky light level
    Brightness BRIGHTNESSMAX = 15;    //Maximum brightness
    Brightness BRIGHTNESSMIN = 2;     //Mimimum brightness
    Brightness BRIGHTNESSDEC = 1;     //Brightness decrease

    std::vector<std::shared_ptr<Chunk>> chunks{};
    Chunk *cpCachePtr = nullptr;
    chunkid cpCacheID = 0;
    ChunkPtrArray cpArray;
    int cloud[128][128];
    int rebuiltChunks, rebuiltChunksCount;
    int updatedChunks, updatedChunksCount;
    int unloadedChunks, unloadedChunksCount;
    int chunkBuildRenderList[256][2];
    int chunkBuildRenders;

    OrderedList<int, Int3, 64> ChunkLoadList{};
    OrderedList<int, Chunk *, 64, std::greater> ChunkUnloadList{};

    void Init() {
        std::stringstream ss;
        ss << "Worlds/" << worldname << "/";
        NEWorld::filesystem::create_directories(ss.str());
        ss.clear();
        ss.str("");
        ss << "Worlds/" << worldname << "/chunks";
        NEWorld::filesystem::create_directories(ss.str());

        cpCachePtr = nullptr;
        cpCacheID = 0;

        cpArray.Create((viewdistance + 2) * 2);
    }

    auto LowerChunkBound(chunkid cid) noexcept {
        return std::lower_bound(chunks.begin(), chunks.end(), cid, [](auto &left, auto right) noexcept {
            return left->GetId() < right;
        });
    }

    Chunk *AddChunk(Int3 vec) {
        const auto cid = GetChunkId(vec);  //Chunk ID
        const auto chunkIter = LowerChunkBound(cid);
        if (chunkIter != chunks.end()) {
            if ((*chunkIter)->GetId() == cid) {
                printf("[Console][Error]");
                printf("Chunk(%d,%d,%d)has been loaded,when adding Chunk.\n", vec.X, vec.Y, vec.Z);
                return chunkIter->get();
            }
        }
        // TODO(Actually try to load from disk)
        const auto newChunk = TerrainGen::Generate::Get().Run(vec);
        const auto& ptr = *chunks.insert(chunkIter, std::shared_ptr<Chunk>(newChunk));
        WorldRenderer::ChunksRenderer::Default().Add(ptr);
        cpCacheID = cid;
        cpCachePtr = newChunk;
        cpArray.Add(newChunk, vec);
        return newChunk;
    }

    Chunk *GetChunk(Int3 vec) {
        const auto cid = GetChunkId(vec);
        if (cpCacheID == cid && cpCachePtr) return cpCachePtr;
        auto ret = cpArray.Get(vec);
        if (ret) {
            cpCacheID = cid;
            cpCachePtr = ret;
            return ret;
        }
        if (!chunks.empty()) {
            const auto iter = LowerChunkBound(cid);
            if (iter != chunks.end()) {
                const auto& chunk = *iter;
                if (chunk->GetId() == cid) {
                    ret = chunk.get();
                    cpCacheID = cid;
                    cpCachePtr = ret;
                    cpArray.Add(ret, vec);
                    return ret;
                }
            }
        }
        return nullptr;
    }

    void DeleteChunk(Int3 vec) {
        const auto id = GetChunkId(vec);  //Chunk ID
        const auto chunkIter = LowerChunkBound(id);
        if (chunkIter != chunks.end()) {
            if ((*chunkIter)->GetId() == id) {
                const auto chunk = chunkIter->get();
                if (cpCachePtr == chunk) {
                    cpCacheID = 0;
                    cpCachePtr = nullptr;
                }
                cpArray.Remove(vec);
                chunks.erase(chunkIter);
            }
        }
    }
    
    std::vector<BoundingBox> getHitboxes(const BoundingBox& box) {
        std::vector<BoundingBox> ret;

        for (auto a = int(box.min.values[0] + 0.5) - 1; a <= int(box.max.values[0] + 0.5) + 1; a++) {
            for (auto b = int(box.min.values[1] + 0.5) - 1; b <= int(box.max.values[1] + 0.5) + 1; b++) {
                for (auto c = int(box.min.values[2] + 0.5) - 1; c <= int(box.max.values[2] + 0.5) + 1; c++) {
                    Int3 pos = { a, b, c };
                    if (BlockInfo(GetBlock(pos)).isSolid()) {
                        auto blockBox = AABB::BoxForBlock(pos);
                        if (AABB::Intersect(box, blockBox)) ret.push_back(blockBox);
                    }
                }
            }
        }
        return ret;
    }

    bool inWater(const BoundingBox &box) {
        for (auto a = int(box.min.values[0] + 0.5) - 1; a <= int(box.max.values[0] + 0.5) + 1; a++) {
            for (auto b = int(box.min.values[1] + 0.5) - 1; b <= int(box.max.values[1] + 0.5) + 1; b++) {
                for (auto c = int(box.min.values[2] + 0.5) - 1; c <= int(box.max.values[2] + 0.5) + 1; c++) {
                    Int3 pos = { a,b,c };
                    auto block = GetBlock(pos);
                    if (block == Blocks::WATER || block == Blocks::LAVA) {
                        if (AABB::Intersect(box, AABB::BoxForBlock(pos))) return true;
                    }
                }
            }
        }
        return false;
    }

    void updateblock(int x, int y, int z, bool blockchanged, int depth) {
        //Blockupdate

        if (depth > 200) return;
        depth++;

        auto updated = blockchanged;
        const auto cx = GetChunkPos(x);
        const auto cy = GetChunkPos(y);
        const auto cz = GetChunkPos(z);

        if (ChunkOutOfBound({cx, cy, cz})) return;

        const auto b = GetBlockPos(Int3{x, y, z});

        auto cptr = GetChunk({cx, cy, cz});
        if (cptr != nullptr) {
            const auto oldbrightness = cptr->GetBrightness(b);
            auto skylighted = true;
            auto yi = y + 1;
            auto cyi = GetChunkPos(yi);
            if (y < 0) skylighted = false;
            else {
                while (!ChunkOutOfBound({(cx), (cyi + 1), (cz)}) && ChunkLoaded({(cx), (cyi + 1), (cz)}) &&
                       skylighted) {
                    if (BlockInfo(GetBlock({x, yi, z})).isOpaque() || GetBlock({(x), (yi), (z)}) == Blocks::WATER) {
                        skylighted = false;
                    }
                    yi++;
                    cyi = GetChunkPos(yi);
                }
            }

            if (!BlockInfo(GetBlock({x, y, z})).isOpaque()) {
                Block blks[7] = {0,
                                 (GetBlock({(x), (y), (z + 1)})),    //Front face
                                 (GetBlock({(x), (y), (z - 1)})),    //Back face
                                 (GetBlock({(x + 1), (y), (z)})),    //Right face
                                 (GetBlock({(x - 1), (y), (z)})),    //Left face
                                 (GetBlock({(x), (y + 1), (z)})),    //Top face
                                 (GetBlock({(x), (y - 1), (z)}))};  //Bottom face
                Brightness brts[7] = {0,
                                      getbrightness(x, y, z + 1),    //Front face
                                      getbrightness(x, y, z - 1),    //Back face
                                      getbrightness(x + 1, y, z),    //Right face
                                      getbrightness(x - 1, y, z),    //Left face
                                      getbrightness(x, y + 1, z),    //Top face
                                      getbrightness(x, y - 1, z)};  //Bottom face
                auto maxbrightness = 1;
                for (auto i = 2; i <= 6; i++) {
                    if (brts[maxbrightness] < brts[i]) maxbrightness = i;
                }
                auto br = brts[maxbrightness];
                if (blks[maxbrightness] == Blocks::WATER) {
                    if (br - 2 < BRIGHTNESSMIN) br = BRIGHTNESSMIN; else br -= 2;
                } else {
                    if (br - 1 < BRIGHTNESSMIN) br = BRIGHTNESSMIN; else br--;
                }

                if (skylighted) {
                    if (br < skylight) br = skylight;
                }
                if (br < BRIGHTNESSMIN) br = BRIGHTNESSMIN;
                //Set brightness
                cptr->SetBrightness(b, br);

            } else {

                //Opaque block
                cptr->SetBrightness(b, 0);
                if (GetBlock({x, y, z}) == Blocks::GLOWSTONE || GetBlock({x, y, z}) == Blocks::LAVA) {
                    cptr->SetBrightness(b, BRIGHTNESSMAX);
                }

            }

            if (oldbrightness != cptr->GetBrightness(b)) updated = true;

            if (updated) {
                updateblock(x, y + 1, z, false, depth);
                updateblock(x, y - 1, z, false, depth);
                updateblock(x + 1, y, z, false, depth);
                updateblock(x - 1, y, z, false, depth);
                updateblock(x, y, z + 1, false, depth);
                updateblock(x, y, z - 1, false, depth);
            }

            setChunkUpdated(cx, cy, cz, true);
            if (b.X == 15 && cx < worldsize - 1) setChunkUpdated(cx + 1, cy, cz, true);
            if (b.X == 0 && cx > -worldsize) setChunkUpdated(cx - 1, cy, cz, true);
            if (b.Y == 15 && cy < worldheight - 1) setChunkUpdated(cx, cy + 1, cz, true);
            if (b.Y == 0 && cy > -worldheight) setChunkUpdated(cx, cy - 1, cz, true);
            if (b.Z == 15 && cz < worldsize - 1) setChunkUpdated(cx, cy, cz + 1, true);
            if (b.Z == 0 && cz > -worldsize) setChunkUpdated(cx, cy, cz - 1, true);

        }
    }

    bool ChunkHintMatch(const Int3 c, Chunk *const cptr) noexcept { return cptr && cptr->GetPosition() == c; }

    Block GetBlock(const Int3 v, Block mask, Chunk *const hint) {
        //获取方块
        const auto c = GetChunkPos(v);
        if (ChunkOutOfBound(c)) return Blocks::ENV;
        const auto b = GetBlockPos(v);
        if (ChunkHintMatch(c, hint)) { return hint->GetBlock(b); }
        const auto ci = GetChunk(c);
        if (ci) { return ci->GetBlock(b); }
        return mask;
    }

    Brightness GetBrightness(Int3 v, Chunk *const hint) {
        //获取亮度
        const auto c = GetChunkPos(v);
        if (ChunkOutOfBound(c)) return skylight;
        const auto b = GetBlockPos(v);
        if (ChunkHintMatch(c, hint)) { return hint->GetBrightness(b); }
        const auto ci = GetChunk(c);
        if (ci) { return ci->GetBrightness(b); }
        return skylight;
    }

    Chunk *GetChunkNoneLazy(const Int3 c) noexcept {
        return GetChunk(c);
    }

    void SetBlock(Int3 v, Block block, Chunk *hint) {
        //设置方块
        const auto c = GetChunkPos(v);
        const auto b = GetBlockPos(v);
        if (ChunkHintMatch(c, hint)) {
            hint->SetBlock(b, block);
            updateblock(v.X, v.Y, v.Z, true);
        } else if (!ChunkOutOfBound(c)) {
            if (const auto i = GetChunkNoneLazy(c); i) {
                i->SetBlock(b, block);
                updateblock(v.X, v.Y, v.Z, true);
            }
        }
    }

    void SetBrightness(Int3 v, Brightness brightness, Chunk *hint) {
        //设置亮度
        const auto c = GetChunkPos(v);
        const auto b = GetBlockPos(v);
        if (ChunkHintMatch(c, hint)) {
            hint->SetBrightness(b, brightness);
        } else if (!ChunkOutOfBound(c)) {
            if (const auto i = GetChunkNoneLazy(c); i) {
                i->SetBrightness(b, brightness);
            }
        }
    }

    bool chunkUpdated(const Int3 vec) {
        const auto i = GetChunk(vec);
        if (!i) return false;
        return i->updated;
    }

    void setChunkUpdated(int x, int y, int z, bool value) {
        if (const auto i = GetChunkNoneLazy({x, y, z}); i) {
            i->updated = value;
        }
    }

    static constexpr auto ccOffset = Int3(7); // offset to a chunk center

    void sortChunkLoadUnloadList(Int3 pos) {
        const auto cp = GetChunkPos(pos);

        ChunkUnloadList.Clear();
        for (auto &chunk : chunks) {
            const auto c = chunk->GetPosition();
            if (ChebyshevDistance(c, cp) > viewdistance)
                ChunkUnloadList.Insert(DistanceSquared(c * 16 + ccOffset, pos), chunk.get());
        }

        ChunkLoadList.Clear();
        const auto diff = Int3(viewdistance + 1);
        Cursor(cp - diff, cp + diff, [&](const auto &c) noexcept {
            if (ChunkOutOfBound(c)) return;
            if (!cpArray.Get(c))
                ChunkLoadList.Insert(DistanceSquared(c * 16 + ccOffset, pos), c);
        });
    }

    void saveAllChunks() {
    }

    void destroyAllChunks() {
        chunks.clear();
        chunks.shrink_to_fit();

        cpArray.Finalize();

        rebuiltChunks = 0;
        rebuiltChunksCount = 0;

        updatedChunks = 0;
        updatedChunksCount = 0;

        unloadedChunks = 0;
        unloadedChunksCount = 0;

        ChunkLoadList.Clear();
        ChunkUnloadList.Clear();

        chunkBuildRenders = 0;
    }

    void buildtree(Int3 pos) {
        auto [x, y, z] = pos.Data;
        //对生成条件进行更严格的检测
        //一：正上方五格必须为空气
        for (auto i = y + 1; i < y + 6; i++) {
            if (GetBlock({(x), (i), (z)}) != Blocks::ENV)return;
        }
        //二：周围五格不能有树
        for (auto ix = x - 4; ix < x + 4; ix++) {
            for (auto iy = y - 4; iy < y + 4; iy++) {
                for (auto iz = z - 4; iz < z + 4; iz++) {
                    if (GetBlock({(ix), (iy), (iz)}) == Blocks::WOOD ||
                        GetBlock({(ix), (iy), (iz)}) == Blocks::LEAF)
                        return;
                }
            }
        }
        //终于可以开始生成了
        //设置泥土
        SetBlock({x, y, z}, Blocks::DIRT);
        //设置树干
        auto h = 0;//高度
        //测算泥土数量
        auto Dirt = 0;//泥土数
        for (auto ix = x - 4; ix < x + 4; ix++) {
            for (auto iy = y - 4; iy < y; iy++) {
                for (auto iz = z - 4; iz < z + 4; iz++) {
                    if (GetBlock({(ix), (iy), (iz)}) == Blocks::DIRT)Dirt++;
                }
            }
        }
        //测算最高高度
        for (auto i = y + 1; i < y + 16; i++) {
            if (GetBlock({(x), (i), (z)}) == Blocks::ENV) { h++; }
            else { break; };
        }
        //取最小值
        h = std::min(h, int(Dirt * 15 / 268 * std::max(rnd(), 0.8)));
        if (h < 7)return;
        //开始生成树干
        for (auto i = y + 1; i < y + h + 1; i++) {
            SetBlock({(x), (i), (z)}, Blocks::WOOD);
        }
        //设置树叶及枝杈
        //计算树叶起始生成高度
        const auto leafh = int(double(h) * 0.618) + 1;//黄金分割比大法好！！！
        const auto distancen2 = int(double((h - leafh + 1) * (h - leafh + 1))) + 1;
        for (auto iy = y + leafh; iy < y + int(double(h) * 1.382) + 2; iy++) {
            for (auto ix = x - 6; ix < x + 6; ix++) {
                for (auto iz = z - 6; iz < z + 6; iz++) {
                    const auto distancen = DistanceSquare(ix, iy, iz, x, y + leafh + 1, z);
                    if ((GetBlock({(ix), (iy), (iz)}) == Blocks::ENV) && (distancen < distancen2)) {
                        if ((distancen <= distancen2 / 9) && (rnd() > 0.3)) {
                            SetBlock({(ix), (iy), (iz)}, Blocks::WOOD);//生成枝杈
                        } else {
                            SetBlock({(ix), (iy), (iz)}, Blocks::LEAF); //生成树叶
                        }
                    }
                }
            }
        }
        // TODO(move this function when terrain carving for terrain generation is possible)
    }

    void explode(int x, int y, int z, int r, Chunk *c) {
        const double maxdistsqr = r * r;
        for (auto fx = x - r - 1; fx < x + r + 1; fx++) {
            for (auto fy = y - r - 1; fy < y + r + 1; fy++) {
                for (auto fz = z - r - 1; fz < z + r + 1; fz++) {
                    const auto distsqr = (fx - x) * (fx - x) + (fy - y) * (fy - y) + (fz - z) * (fz - z);
                    if (distsqr <= maxdistsqr * 0.75 ||
                        distsqr <= maxdistsqr && rnd() > (distsqr - maxdistsqr * 0.6) / (maxdistsqr * 0.4)) {
                        const auto e = GetBlock({(fx), (fy), (fz)});
                        if (e == Blocks::ENV) continue;
                        for (auto j = 1; j <= 12; j++) {
                            Particles::throwParticle(e,
                                                     float(fx + rnd() - 0.5f), float(fy + rnd() - 0.2f),
                                                     float(fz + rnd() - 0.5f),
                                                     float(rnd() * 0.2f - 0.1f), float(rnd() * 0.2f - 0.1f),
                                                     float(rnd() * 0.2f - 0.1f),
                                                     float(rnd() * 0.02 + 0.03), int(rnd() * 60) + 30);
                        }
                        SetBlock({(fx), (fy), (fz)}, Blocks::ENV, c);
                    }
                }
            }
        }
    }

    void PutBlock(const Int3 v, Block block) {
        auto &blockInfo = BlockInfo(block);
        if (blockInfo.BeforeBlockPlace(v, block)) {
            SetBlock(v, block);
            blockInfo.AfterBlockPlace(v, block);
        }
    }

    void PickBlock(const Int3 v) {
        const auto block = GetBlock(v);
        auto &type = BlockInfo(block);
        if (type.BeforeBlockDestroy(v, block)) {
            SetBlock(v, Blocks::ENV);
            type.AfterBlockDestroy(v, block);
        }
    }

}
