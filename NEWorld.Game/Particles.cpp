#include "Particles.h"
#include "Universe/World/World.h"
#include "Textures.h"
#include "Renderer/BufferBuilder.h"
#include "Renderer/GL/Pipeline.h"

namespace Particles {
    std::vector<Particle> ptcs;
    int ptcsrendered;
    double pxpos, pypos, pzpos;

    Renderer::Pipeline &GetPipeline() {
        static auto pipeline = []() {
            using namespace Renderer;
            constexpr int sof = sizeof(float);
            PipelineBuilder builder{Topology::TriangleList};
            builder.SetBinding(1, 7 * sof, 0);
            builder.AddAttribute(DataType::Float32, 1, 1, 2, 0);
            builder.AddAttribute(DataType::Float32, 1, 2, 2, 2 * sof);
            builder.AddAttribute(DataType::Float32, 1, 3, 3, 4 * sof);
            builder.SetShader(ShaderType::Vertex,
                              CompileFile(ShaderType::Vertex, "./Assets/Shaders/Particle.vsh", {}));
            builder.SetShader(ShaderType::Fragment,
                              CompileFile(ShaderType::Fragment, "./Assets/Shaders/Particle.fsh", {}));
            auto result = builder.Build();
            result->SetUniform(0, 0);
            result->BindIndexBuffer(GetDefaultQuadIndex(), IndexType::U32);
            return result;
        }();
        return pipeline;
    }

    void update(Particle &ptc) {
        const auto psz = ptc.psize;
        ptc.hb.min.values[0] = ptc.xpos - psz;
        ptc.hb.max.values[0] = ptc.xpos + psz;
        ptc.hb.min.values[1] = ptc.ypos - psz;
        ptc.hb.min.values[1] = ptc.ypos + psz;
        ptc.hb.min.values[2] = ptc.zpos - psz;
        ptc.hb.min.values[2] = ptc.zpos + psz;

        double dx = ptc.xsp;
        double dy = ptc.ysp;
        double dz = ptc.zsp;

        auto Hitboxes = World::getHitboxes(ptc.hb.extend(AABB::Move(ptc.hb, { dx, dy, dz })));
        const int hitnum = Hitboxes.size();
        for (auto i = 0; i < hitnum; i++) {
            dy = AABB::MaxMove(ptc.hb, Hitboxes[i], dy, 1);
        }
        ptc.hb = AABB::Move(ptc.hb, { 0.0, dy, 0.0 });
        for (auto i = 0; i < hitnum; i++) {
            dx = AABB::MaxMove(ptc.hb, Hitboxes[i], dx,0);
        }
        ptc.hb = AABB::Move(ptc.hb, { dx, 0.0, 0.0 });
        for (auto i = 0; i < hitnum; i++) {
            dz = AABB::MaxMove(ptc.hb, Hitboxes[i], dz, 2);
        }
        ptc.hb = AABB::Move(ptc.hb, { 0.0, 0.0, dz });

        ptc.xpos += dx;
        ptc.ypos += dy;
        ptc.zpos += dz;
        if (dy != ptc.ysp) ptc.ysp = 0.0;
        ptc.xsp *= 0.6f;
        ptc.zsp *= 0.6f;
        ptc.ysp -= 0.01f;
        ptc.lasts -= 1;

    }

    void updateall() {
        for (auto iter = ptcs.begin(); iter < ptcs.end();) {
            if (!iter->exist) continue;
            update(*iter);
            if (iter->lasts <= 0) {
                iter->exist = false;
                iter = ptcs.erase(iter);
            } else {
                iter++;
            }
        }
    }

    void render(Renderer::BufferBuilder<> &builder, Particle &ptc) {
        //if (!Frustum::aabbInFrustum(ptc.hb)) return;
        ptcsrendered++;
        const auto size =
                static_cast<float>(BLOCKTEXTURE_UNITSIZE) / BLOCKTEXTURE_SIZE * (ptc.psize <= 1.0f ? ptc.psize : 1.0f);
        const auto col = World::getbrightness(RoundInt(ptc.xpos), RoundInt(ptc.ypos), RoundInt(ptc.zpos)) /
                         static_cast<float>(World::BRIGHTNESSMAX);
        const auto col1 = col * 0.5f;
        const auto col2 = col * 0.7f;
        const auto tcx = ptc.tcX;
        const auto tcy = ptc.tcY;
        const auto psize = ptc.psize;
        const auto palpha = (ptc.lasts < 30 ? ptc.lasts / 30.0 : 1.0);
        const auto xpos = ptc.xpos - pxpos;
        const auto ypos = ptc.ypos - pypos;
        const auto zpos = ptc.zpos - pzpos;

        builder.put<24>(
                tcx, tcy, col1, palpha, xpos - psize, ypos - psize, zpos + psize,
                tcx + size, tcy, col1, palpha, xpos + psize, ypos - psize, zpos + psize,
                tcx + size, tcy + size, col1, palpha, xpos + psize, ypos + psize, zpos + psize,
                tcx, tcy + size, col1, palpha, xpos - psize, ypos + psize, zpos + psize,

                tcx, tcy, col1, palpha, xpos - psize, ypos + psize, zpos - psize,
                tcx + size, tcy, col1, palpha, xpos + psize, ypos + psize, zpos - psize,
                tcx + size, tcy + size, col1, palpha, xpos + psize, ypos - psize, zpos - psize,
                tcx, tcy + size, col1, palpha, xpos - psize, ypos - psize, zpos - psize,

                tcx, tcy, col, palpha, xpos + psize, ypos + psize, zpos - psize,
                tcx + size, tcy, col, palpha, xpos - psize, ypos + psize, zpos - psize,
                tcx + size, tcy + size, col, palpha, xpos - psize, ypos + psize, zpos + psize,
                tcx, tcy + size, col, palpha, xpos + psize, ypos + psize, zpos + psize,

                tcx, tcy, col, palpha, xpos - psize, ypos - psize, zpos - psize,
                tcx + size, tcy, col, palpha, xpos + psize, ypos - psize, zpos - psize,
                tcx + size, tcy + size, col, palpha, xpos + psize, ypos - psize, zpos + psize,
                tcx, tcy + size, col, palpha, xpos - psize, ypos - psize, zpos + psize,

                tcx, tcy, col2, palpha, xpos + psize, ypos + psize, zpos - psize,
                tcx + size, tcy, col2, palpha, xpos + psize, ypos + psize, zpos + psize,
                tcx + size, tcy + size, col2, palpha, xpos + psize, ypos - psize, zpos + psize,
                tcx, tcy + size, col2, palpha, xpos + psize, ypos - psize, zpos - psize,

                tcx, tcy, col2, palpha, xpos - psize, ypos - psize, zpos - psize,
                tcx + size, tcy, col2, palpha, xpos - psize, ypos - psize, zpos + psize,
                tcx + size, tcy + size, col2, palpha, xpos - psize, ypos + psize, zpos + psize,
                tcx, tcy + size, col2, palpha, xpos - psize, ypos + psize, zpos - psize
        );
    }

    void renderall(double xpos, double ypos, double zpos) {
        pxpos = xpos;
        pypos = ypos;
        pzpos = zpos;
        ptcsrendered = 0;
        Renderer::BufferBuilder builder{};
        for (auto &ptc: ptcs) {
            if (!ptc.exist) continue;
            render(builder, ptc);
        }
        VBOID vbo{0};
        vtxCount vts{0};
        builder.flush(vbo, vts);
        if (vts != 0) {
            GetPipeline()->Use();
            GetPipeline()->BindVertexBuffer(1, vbo, 0);
            GetPipeline()->DrawIndexed(vts + vts / 2, 0);
            glDeleteBuffers(1, &vbo);
            glBindVertexArray(0);
        }
    }

    void throwParticle(Block pt, float x, float y, float z, float xs, float ys, float zs, float psz, int last) {
        const auto tcX1 = static_cast<float>(Textures::getTexcoordX(pt, 2));
        const auto tcY1 = static_cast<float>(Textures::getTexcoordY(pt, 2));
        Particle ptc;
        ptc.exist = true;
        ptc.xpos = x;
        ptc.ypos = y;
        ptc.zpos = z;
        ptc.xsp = xs;
        ptc.ysp = ys;
        ptc.zsp = zs;
        ptc.psize = psz;
        ptc.hb.min.values[0] = x - psz;
        ptc.hb.max.values[0] = x + psz;
        ptc.hb.min.values[1] = y - psz;
        ptc.hb.max.values[1] = y + psz;
        ptc.hb.min.values[2] = z - psz;
        ptc.hb.max.values[2] = z + psz;
        ptc.lasts = last;
        ptc.tcX = tcX1 + static_cast<float>(rnd()) * (static_cast<float>(BLOCKTEXTURE_UNITSIZE) / BLOCKTEXTURE_SIZE) *
                         (1.0f - psz);
        ptc.tcY = tcY1 + static_cast<float>(rnd()) * (static_cast<float>(BLOCKTEXTURE_UNITSIZE) / BLOCKTEXTURE_SIZE) *
                         (1.0f - psz);
        ptcs.push_back(ptc);
    }
}
