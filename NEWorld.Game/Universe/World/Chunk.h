#pragma once

#include "Definitions.h"
#include "Blocks.h"
#include "Frustum.h"
#include <cstring>
#include <Math/Vector3.h>
#include "Data/ChunkStorage.h"
#include "Universe/Entity/bvh.h"

class Object;

namespace World {
    extern std::string worldname;
    extern Brightness BRIGHTNESSMIN;
    extern Brightness skylight;

    class Chunk {
    private:
        Data::ChunkStorage mBlock{4, Data::Init};
        Brightness *mBrightness;
        std::vector<Object *> objects;
        chunkid mId;
        const Int3 mPos;
        BoundingBox mBounds;
        static double relBaseX, relBaseY, relBaseZ;
        static Frustum TestFrustum;

        static constexpr unsigned int GetIndex(const Int3 vec) noexcept {
            const auto v = UInt3(vec);
            return (v.X << ChunkPlaneSizeLog2) | (v.Y << ChunkEdgeSizeLog2) | v.Z;
        }

    public:
        //竟然一直都没有构造函数/析构函数 还要手动调用Init...我受不了啦(╯‵□′)╯︵┻━┻ --Null
        //2333 --qiaozhanrong
        Chunk(int cxi, int cyi, int czi, chunkid idi) : mPos(cxi, cyi, czi), mId(idi),
                                                        Modified(false), Empty(false), updated(false),
                                                        renderBuilt(false), DetailGenerated(false), loadAnim(0.0) {
            memset(vertexes, 0, sizeof(vertexes));
            memset(vbuffer, 0, sizeof(vbuffer));
        }

        [[nodiscard]] Int3 GetPosition() const noexcept { return mPos; }

        bool Empty, updated, renderBuilt, Modified, DetailGenerated;
        vtxCount vertexes[4];
        VBOID vbuffer[4];
        double loadAnim;
        bool visible;

        [[nodiscard]] chunkid GetId() const noexcept { return mId; }

        void create();

        void destroy();

        void Load(bool initIfEmpty = true);

        ~Chunk();

        void buildTerrain(bool initIfEmpty = true);

        void buildDetail();

        void build(bool initIfEmpty = true);

        bool LoadFromFile(); //返回true代表区块文件打开成功

        void SaveToFile();

        void buildRender();

        void destroyRender();

        Block GetBlock(const Int3 vec) noexcept {return mBlock.Get(GetIndex(vec));}

        Brightness GetBrightness(const Int3 vec) noexcept {return mBrightness[GetIndex(vec)];}

        void SetBlock(const Int3 vec, Block block) noexcept {
            mBlock.Set(GetIndex(vec), block);
            Modified = true;
        }

        void SetBrightness(const Int3 vec, Brightness brightness) noexcept {
            mBrightness[GetIndex(vec)] = brightness;
            Modified = true;
        }

        static void setRelativeBase(double x, double y, double z, Frustum &frus) {
            relBaseX = x;
            relBaseY = y;
            relBaseZ = z;
            TestFrustum = frus;
        }

        BoundingBox getBaseAABB();

        Frustum::ChunkBox getRelativeAABB();

        void calcVisible() { visible = TestFrustum.FrustumTest(getRelativeAABB()); }

    };

    using ChunkGenerator = void (*)(Chunk&);

    void UseChunkGenerator(ChunkGenerator newGenerator) noexcept;
}
