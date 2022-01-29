#include "ChunkRenderer.h"
#include "Renderer/Renderer.h"
#include "Universe/World/World.h"
#include "Renderer/BufferBuilder.h"
#include "Math/Vector4.h"
#include "Dispatch.h"

namespace WorldRenderer {
    class ChunkRenderContext {
        static constexpr auto MX = 18 * 18;
        static constexpr auto MY = 18;
    public:
        ChunkRenderContext(World::Chunk *chunk) {
            Block *bF = mBStates;
            std::uint8_t *lF = mLumin;
            const auto cPos = chunk->GetPosition() * 16;
            Cursor(cPos - Int3{1}, cPos + Int3{17}, [&bF, &lF, chunk](const auto &v) {
                *(bF++) = World::GetBlock(v, Blocks::ROCK, chunk);
                *(lF++) = World::GetBrightness(v);
            });
        }

        void Rebase(int x, int y, int z) noexcept { mBase = (x + 1) * MX + (y + 1) * MY + (z + 1); }

        auto State(int dx, int dy, int dz) noexcept { return mBStates[mBase + dx * MX + dy * MY + dz]; }

        Brightness Lumin(int dx, int dy, int dz) noexcept { return mLumin[mBase + dx * MX + dy * MY + dz]; }

    private:
        int mBase{0};
        Block mBStates[18 * 18 * 18];
        std::uint8_t mLumin[18 * 18 * 18]{0};
    };

    enum Face {
        Front, Back, Right, Left, Top, Bottom
    };

    //TODO(simplify this function)
    static void renderblock(Renderer::BufferBuilder<> &builder, ChunkRenderContext &ctx, int x, int y, int z) {
        double tcx, tcy, size, EPS = 0.0;
        Block blk[7] = {ctx.State(0, 0, 0), ctx.State(0, 0, 1), ctx.State(0, 0, -1),
                        ctx.State(1, 0, 0), ctx.State(-1, 0, 0), ctx.State(0, 1, 0), ctx.State(0, -1, 0)};
        Brightness brt[7] = {ctx.Lumin(0, 0, 0), ctx.Lumin(0, 0, 1), ctx.Lumin(0, 0, -1),
                             ctx.Lumin(1, 0, 0), ctx.Lumin(-1, 0, 0), ctx.Lumin(0, 1, 0), ctx.Lumin(0, -1, 0)};

        size = 1 / 8.0f - EPS;

        if (NiceGrass && blk[0] == Blocks::GRASS && ctx.State(0, -1, 1) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Front Face
        if (!(BlockInfo(blk[1]).isOpaque() || (blk[1] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            Double4 col{double(brt[1])};

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting)
                col = (col + Double4(
                        ctx.Lumin(0, -1, 1) + ctx.Lumin(-1, 0, 1) + ctx.Lumin(-1, -1, 1),
                        ctx.Lumin(0, -1, 1) + ctx.Lumin(1, 0, 1) + ctx.Lumin(1, -1, 1),
                        ctx.Lumin(0, 1, 1) + ctx.Lumin(1, 0, 1) + ctx.Lumin(1, 1, 1),
                        ctx.Lumin(0, 1, 1) + ctx.Lumin(-1, 0, 1) + ctx.Lumin(-1, 1, 1)
                )) / 4.0;

            col /= World::BRIGHTNESSMAX;

            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) col *= 0.5;
            // att, tex, col vert
            builder.put<4>(
                    0.0f, tcx, tcy, col.X, col.X, col.X, -0.5 + x, -0.5 + y, 0.5 + z,
                    0.0f, tcx + size, tcy, col.Y, col.Y, col.Y, 0.5 + x, -0.5 + y, 0.5 + z,
                    0.0f, tcx + size, tcy + size, col.Z, col.Z, col.Z, 0.5 + x, 0.5 + y, 0.5 + z,
                    0.0f, tcx, tcy + size, col.W, col.W, col.W, -0.5 + x, 0.5 + y, 0.5 + z
            );
        }

        if (NiceGrass && blk[0] == Blocks::GRASS && ctx.State(0, -1, -1) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Back Face
        if (!(BlockInfo(blk[2]).isOpaque() || (blk[2] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            Double4 col{double(brt[2])};

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting)
                col = (col + Double4(
                        ctx.Lumin(0, -1, -1) + ctx.Lumin(-1, 0, -1) + ctx.Lumin(-1, -1, -1),
                        ctx.Lumin(0, 1, -1) + ctx.Lumin(-1, 0, -1) + ctx.Lumin(-1, 1, -1),
                        ctx.Lumin(0, 1, -1) + ctx.Lumin(1, 0, -1) + ctx.Lumin(1, 1, -1),
                        ctx.Lumin(0, -1, -1) + ctx.Lumin(1, 0, -1) + ctx.Lumin(1, -1, -1)
                )) / 4.0;

            col /= World::BRIGHTNESSMAX;
            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) col *= 0.5;
            // att, tex, col vert
            builder.put<4>(
                    1.0f, tcx + size * 1.0, tcy + size * 0.0, col.X, col.X, col.X, -0.5 + x, -0.5 + y, -0.5 + z,
                    1.0f, tcx + size * 1.0, tcy + size * 1.0, col.Y, col.Y, col.Y, -0.5 + x, 0.5 + y, -0.5 + z,
                    1.0f, tcx + size * 0.0, tcy + size * 1.0, col.Z, col.Z, col.Z, 0.5 + x, 0.5 + y, -0.5 + z,
                    1.0f, tcx + size * 0.0, tcy + size * 0.0, col.W, col.W, col.W, 0.5 + x, -0.5 + y, -0.5 + z
            );
        }

        if (NiceGrass && blk[0] == Blocks::GRASS && ctx.State(1, -1, 0) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Right face
        if (!(BlockInfo(blk[3]).isOpaque() || (blk[3] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            Double4 col{double(brt[3])};

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting)
                col = (col + Double4(
                        ctx.Lumin(1, -1, 0) + ctx.Lumin(1, 0, -1) + ctx.Lumin(1, -1, -1),
                        ctx.Lumin(1, 1, 0) + ctx.Lumin(1, 0, -1) + ctx.Lumin(1, 1, -1),
                        ctx.Lumin(1, 1, 0) + ctx.Lumin(1, 0, 1) + ctx.Lumin(1, 1, 1),
                        ctx.Lumin(1, -1, 0) + ctx.Lumin(1, 0, 1) + ctx.Lumin(1, -1, 1)
                )) / 4.0;

            col /= World::BRIGHTNESSMAX;
            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) col *= 0.7;

            // att, tex, col vert
            builder.put<4>(
                    2.0f, tcx + size * 1.0, tcy + size * 0.0, col.X, col.X, col.X, 0.5 + x, -0.5 + y, -0.5 + z,
                    2.0f, tcx + size * 1.0, tcy + size * 1.0, col.Y, col.Y, col.Y, 0.5 + x, 0.5 + y, -0.5 + z,
                    2.0f, tcx + size * 0.0, tcy + size * 1.0, col.Z, col.Z, col.Z, 0.5 + x, 0.5 + y, 0.5 + z,
                    2.0f, tcx + size * 0.0, tcy + size * 0.0, col.W, col.W, col.W, 0.5 + x, -0.5 + y, 0.5 + z
            );
        }

        if (NiceGrass && blk[0] == Blocks::GRASS && ctx.State(-1, -1, 0) == Blocks::GRASS) {
            tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
        } else {
            tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
            tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
        }

        // Left Face
        if (!(BlockInfo(blk[4]).isOpaque() || (blk[4] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            Double4 col{double(brt[4])};

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting)
                col = (col + Double4(
                        ctx.Lumin(-1, -1, 0) + ctx.Lumin(-1, 0, -1) + ctx.Lumin(-1, -1, -1),
                        ctx.Lumin(-1, -1, 0) + ctx.Lumin(-1, 0, 1) + ctx.Lumin(-1, -1, 1),
                        ctx.Lumin(-1, 1, 0) + ctx.Lumin(-1, 0, 1) + ctx.Lumin(-1, 1, 1),
                        ctx.Lumin(-1, 1, 0) + ctx.Lumin(-1, 0, -1) + ctx.Lumin(-1, 1, -1)
                )) / 4.0;

            col /= World::BRIGHTNESSMAX;
            if (blk[0] != Blocks::GLOWSTONE && !Renderer::AdvancedRender) col *= 0.7;

            // att, tex, col vert
            builder.put<4>(
                    3.0f, tcx + size * 0.0, tcy + size * 0.0, col.X, col.X, col.X, -0.5 + x, -0.5 + y, -0.5 + z,
                    3.0f, tcx + size * 1.0, tcy + size * 0.0, col.Y, col.Y, col.Y, -0.5 + x, -0.5 + y, 0.5 + z,
                    3.0f, tcx + size * 1.0, tcy + size * 1.0, col.Z, col.Z, col.Z, -0.5 + x, 0.5 + y, 0.5 + z,
                    3.0f, tcx + size * 0.0, tcy + size * 1.0, col.W, col.W, col.W, -0.5 + x, 0.5 + y, -0.5 + z
            );
        }

        tcx = Textures::getTexcoordX(blk[0], 1);
        tcy = Textures::getTexcoordY(blk[0], 1);

        // Top Face
        if (!(BlockInfo(blk[5]).isOpaque() || (blk[5] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            Double4 col{double(brt[5])};

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting)
                col = (col + Double4(
                        ctx.Lumin(0, 1, -1) + ctx.Lumin(-1, 1, 0) + ctx.Lumin(-1, 1, -1),
                        ctx.Lumin(0, 1, 1) + ctx.Lumin(-1, 1, 0) + ctx.Lumin(-1, 1, 1),
                        ctx.Lumin(0, 1, 1) + ctx.Lumin(1, 1, 0) + ctx.Lumin(1, 1, 1),
                        ctx.Lumin(0, 1, -1) + ctx.Lumin(1, 1, 0) + ctx.Lumin(1, 1, -1)
                )) / 4.0;

            col /= World::BRIGHTNESSMAX;

            // att, tex, col vert
            builder.put<4>(
                    4.0f, tcx + size * 0.0, tcy + size * 1.0, col.X, col.X, col.X, -0.5 + x, 0.5 + y, -0.5 + z,
                    4.0f, tcx + size * 0.0, tcy + size * 0.0, col.Y, col.Y, col.Y, -0.5 + x, 0.5 + y, 0.5 + z,
                    4.0f, tcx + size * 1.0, tcy + size * 0.0, col.Z, col.Z, col.Z, 0.5 + x, 0.5 + y, 0.5 + z,
                    4.0f, tcx + size * 1.0, tcy + size * 1.0, col.W, col.W, col.W, 0.5 + x, 0.5 + y, -0.5 + z
            );
        }

        tcx = Textures::getTexcoordX(blk[0], 3);
        tcy = Textures::getTexcoordY(blk[0], 3);

        // Bottom Face
        if (!(BlockInfo(blk[6]).isOpaque() || (blk[6] == blk[0] && !BlockInfo(blk[0]).isOpaque())) ||
            blk[0] == Blocks::LEAF) {

            Double4 col{double(brt[6])};

            if (blk[0] != Blocks::GLOWSTONE && SmoothLighting)
                col = (col + Double4(
                        ctx.Lumin(0, -1, -1) + ctx.Lumin(-1, -1, 0) + ctx.Lumin(-1, -1, -1),
                        ctx.Lumin(0, -1, -1) + ctx.Lumin(1, -1, 0) + ctx.Lumin(1, -1, -1),
                        ctx.Lumin(0, -1, 1) + ctx.Lumin(1, -1, 0) + ctx.Lumin(1, -1, 1),
                        ctx.Lumin(0, -1, 1) + ctx.Lumin(-1, -1, 0) + ctx.Lumin(-1, -1, 1)
                )) / 4.0;

            col /= World::BRIGHTNESSMAX;

            // att, tex, col vert
            builder.put<4>(
                    5.0f, tcx + size * 1.0, tcy + size * 1.0, col.X, col.X, col.X, -0.5 + x, -0.5 + y, -0.5 + z,
                    5.0f, tcx + size * 0.0, tcy + size * 1.0, col.Y, col.Y, col.Y, 0.5 + x, -0.5 + y, -0.5 + z,
                    5.0f, tcx + size * 0.0, tcy + size * 0.0, col.Z, col.Z, col.Z, 0.5 + x, -0.5 + y, 0.5 + z,
                    5.0f, tcx + size * 1.0, tcy + size * 0.0, col.W, col.W, col.W, -0.5 + x, -0.5 + y, 0.5 + z
            );
        }
    }

    static void RenderPrimitive_Depth(Renderer::BufferBuilder<> &builder, QuadPrimitive_Depth &p) {
        const auto x = p.x, y = p.y, z = p.z, length = p.length;
        switch (p.direction) {
            case 0:
                return builder.put<4>(x + 0.5, y - 0.5, z - 0.5, x + 0.5, y + 0.5, z - 0.5,
                                      x + 0.5, y + 0.5, z + length + 0.5, x + 0.5, y - 0.5, z + length + 0.5);
            case 1:
                return builder.put<4>(x - 0.5, y + 0.5, z - 0.5, x - 0.5, y - 0.5, z - 0.5,
                                      x - 0.5, y - 0.5, z + length + 0.5, x - 0.5, y + 0.5, z + length + 0.5);
            case 2:
                return builder.put<4>(x + 0.5, y + 0.5, z - 0.5, x - 0.5, y + 0.5, z - 0.5,
                                      x - 0.5, y + 0.5, z + length + 0.5, x + 0.5, y + 0.5, z + length + 0.5);
            case 3:
                return builder.put<4>(x - 0.5, y - 0.5, z - 0.5, x + 0.5, y - 0.5, z - 0.5,
                                      x + 0.5, y - 0.5, z + length + 0.5, x - 0.5, y - 0.5, z + length + 0.5);
            case 4:
                return builder.put<4>(x - 0.5, y + 0.5, z + 0.5, x - 0.5, y - 0.5, z + 0.5,
                                      x + length + 0.5, y - 0.5, z + 0.5, x + length + 0.5, y + 0.5, z + 0.5);
            case 5:
                return builder.put<4>(x - 0.5, y - 0.5, z - 0.5, x - 0.5, y + 0.5, z - 0.5,
                                      x + length + 0.5, y + 0.5, z - 0.5, x + length + 0.5, y - 0.5, z - 0.5);
        }
    }

    static ValueAsync<void> RenderDepthModelEvaluate(World::Chunk *c, Renderer::BufferBuilder<> &builder) {
        co_await SwitchTo(GetSessionDefault());
        const auto cp = c->GetPosition();
        auto x = 0, y = 0, z = 0;
        QuadPrimitive_Depth cur;
        Block bl, neighbour;
        auto valid = false;
        int cur_l_mx = bl = neighbour = 0;
        //Linear merge for depth model
        for (auto d = 0; d < 6; d++) {
            cur.direction = d;
            for (auto i = 0; i < 16; i++)
                for (auto j = 0; j < 16; j++) {
                    for (auto k = 0; k < 16; k++) {
                        //Get position
                        if (d < 2) x = i, y = j, z = k;
                        else if (d < 4) x = i, y = j, z = k;
                        else x = k, y = i, z = j;
                        //Get block ID
                        bl = c->GetBlock({x, y, z});
                        //Get neighbour ID
                        const auto xx = x + delta[d][0], yy = y + delta[d][1], zz = z + delta[d][2];
                        if (xx < 0 || xx >= 16 || yy < 0 || yy >= 16 || zz < 0 || zz >= 16) {
                            neighbour = World::GetBlock(cp * 16 + Int3(xx, yy, zz));
                        } else {
                            neighbour = c->GetBlock({(xx), (yy), (zz)});
                        }
                        //Render
                        if (bl == Blocks::ENV || bl == Blocks::GLASS || bl == neighbour && bl != Blocks::LEAF ||
                            BlockInfo(neighbour).isOpaque() || BlockInfo(bl).isTranslucent()) {
                            //Not valid block
                            if (valid) {
                                if (BlockInfo(neighbour).isOpaque()) {
                                    if (cur_l_mx < cur.length) cur_l_mx = cur.length;
                                    cur_l_mx++;
                                } else {
                                    RenderPrimitive_Depth(builder, cur);
                                    valid = false;
                                }
                            }
                            continue;
                        }
                        if (valid) {
                            if (cur_l_mx > cur.length) cur.length = cur_l_mx;
                            cur.length++;
                        } else {
                            valid = true;
                            cur.x = x;
                            cur.y = y;
                            cur.z = z;
                            cur.length = cur_l_mx = 0;
                        }
                    }
                    if (valid) {
                        RenderPrimitive_Depth(builder, cur);
                        valid = false;
                    }
                }
        }
    }

    static ValueAsync<void> RenderDepthModel(World::Chunk *c, ChunkRender &r) {
        if (Renderer::AdvancedRender) co_return;
        Renderer::BufferBuilder builder{};
        co_await RenderDepthModelEvaluate(c, builder);
        co_await builder.flushAsync(r.Renders[3].Buffer, r.Renders[3].Count);
    }

    static ValueAsync<void> BuildRenderEvaluate(World::Chunk *&c, Renderer::BufferBuilder<> b[]) {
        co_await SwitchTo(GetSessionDefault());
        auto context = temp::make_unique<ChunkRenderContext>(c);
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 16; y++) {
                for (int z = 0; z < 16; z++) {
                    context->Rebase(x, y, z);
                    const auto curr = context->State(0, 0, 0);
                    if (curr == Blocks::ENV) continue;
                    if (!BlockInfo(curr).isTranslucent()) renderblock(b[0], *context, x, y, z);
                    if (BlockInfo(curr).isTranslucent() && BlockInfo(curr).isSolid())
                        renderblock(b[1], *context, x, y, z);
                    if (!BlockInfo(curr).isSolid()) renderblock(b[2], *context, x, y, z);
                }
            }
        }
    }

    static ValueAsync<void> RenderChunk(World::Chunk *c, ChunkRender &r) {
        Renderer::BufferBuilder<> b[3]{};
        co_await BuildRenderEvaluate(c, b);
        co_await Await(
                b[0].flushAsync(r.Renders[0].Buffer, r.Renders[0].Count),
                b[1].flushAsync(r.Renders[1].Buffer, r.Renders[1].Count),
                b[2].flushAsync(r.Renders[2].Buffer, r.Renders[2].Count)
        );
    }

    bool ChunkRender::CheckBuild(const std::shared_ptr<World::Chunk> &c) {
        for (auto x = -1; x <= 1; x++) {
            for (auto y = -1; y <= 1; y++) {
                for (auto z = -1; z <= 1; z++) {
                    if (x == 0 && y == 0 && z == 0) continue;
                    if (World::ChunkOutOfBound(c->GetPosition() + Int3{x, y, z})) continue;
                    if (!World::ChunkLoaded(c->GetPosition() + Int3{x, y, z})) return false;
                }
            }
        }
        return true;
    }

    ValueAsync<void> ChunkRender::Rebuild(std::shared_ptr<World::Chunk> c) {
        World::rebuiltChunks++;
        World::updatedChunks++;
        co_await Await(
                RenderChunk(c.get(), *this),
                RenderDepthModel(c.get(), *this)
        );
        c->updated = false;
        Built = true;
    }
}
