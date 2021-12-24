#pragma once

#include "Definitions.h"
#include "Blocks.h"
#include "Frustum.h"
#include <cstring>
#include <Math/Vector3.h>
#include <System/PmrBase.h>
#include "Data/ChunkStorage.h"
#include "Universe/Entity/bvh.h"

class Object;

namespace World {
    extern std::string worldname;
    extern Brightness BRIGHTNESSMIN;
    extern Brightness BRIGHTNESSMAX;    //Maximum brightness
    extern Brightness skylight;

    constexpr chunkid GetChunkId(Int3 vec) noexcept {
        if (vec.Y == -128) vec.Y = 0;
        if (vec.Y <= 0) vec.Y = abs(vec.Y) + (1LL << 7);
        if (vec.X == -134217728) vec.X = 0;
        if (vec.X <= 0) vec.X = abs(vec.X) + (1LL << 27);
        if (vec.Z == -134217728) vec.Z = 0;
        if (vec.Z <= 0) vec.Z = abs(vec.Z) + (1LL << 27);
        return (chunkid(vec.Y) << 56) + (chunkid(vec.X) << 28) + vec.Z;
    }

    struct ChunkPosHash {
        chunkid operator()(Int3 pos) const noexcept {
            return GetChunkId(pos);
        }
    };

    class ChunkData {
        static constexpr int GetIndex(const Int3 vec) noexcept {
            const auto v = UInt3(vec);
            return static_cast<int>((v.X << ChunkPlaneSizeLog2) | (v.Y << ChunkEdgeSizeLog2) | v.Z);
        }
    public:
        ChunkData() = default;

        ChunkData(const ChunkData& o): mBrightness(o.mBrightness) {
            for (int i = 0; i < ChunkCubicSize; ++i) mBlock.Set(i, o.mBlock.Get(i));
        }

        Block GetBlock(const Int3 vec) noexcept { return mBlock.Get(GetIndex(vec)); }

        Brightness GetBrightness(const Int3 vec) noexcept { return mBrightness[GetIndex(vec)]; }

        void SetBlock(const Int3 vec, Block block) noexcept {
            mBlock.Set(GetIndex(vec), block);
        }

        void SetBrightness(const Int3 vec, Brightness brightness) noexcept {
            mBrightness[GetIndex(vec)] = brightness;
        }

    private:
        std::array<Brightness, 4096> mBrightness{0};
        Data::ChunkStorage mBlock{4, Data::Init};
    };

    class Chunk {
    private:
        bool mLazy;
        ChunkData *mData;
        chunkid mId;
        const Int3 mPos;
        BoundingBox mBounds;
        std::vector<std::shared_ptr<PmrBase>> mAttached {};
        static double relBaseX, relBaseY, relBaseZ;
        static Frustum TestFrustum;
    public:
        Chunk(Int3 pos, ChunkData *data, bool isShared = false) noexcept:
                mPos(pos), mId(GetChunkId(pos)), mLazy(isShared), mData(data),
                Modified(false), Empty(false), updated(false),
                renderBuilt(false), loadAnim(0.0) {
            memset(vertexes, 0, sizeof(vertexes));
            memset(vbuffer, 0, sizeof(vbuffer));
            mBounds = getBaseAABB();
        }

        ~Chunk();

        [[nodiscard]] Int3 GetPosition() const noexcept { return mPos; }

        bool Empty, updated, renderBuilt, Modified;
        vtxCount vertexes[4];
        VBOID vbuffer[4];
        double loadAnim;
        bool visible;

        [[nodiscard]] chunkid GetId() const noexcept { return mId; }

        void buildRender();

        void destroyRender();

        Block GetBlock(const Int3 vec) noexcept { return mData->GetBlock(vec); }

        Brightness GetBrightness(const Int3 vec) noexcept { return mData->GetBrightness(vec); }

        void SetBlock(const Int3 vec, Block block) noexcept {
            if (mLazy) {
                mLazy = false;
                mData = new ChunkData(*mData);
            }
            mData->SetBlock(vec, block);
            Modified = true;
        }

        void SetBrightness(const Int3 vec, Brightness brightness) noexcept {
            if (mLazy) {
                mLazy = false;
                mData = new ChunkData(*mData);
            }
            mData->SetBrightness(vec, brightness);
            Modified = true;
        }

        static void setRelativeBase(double x, double y, double z, Frustum &frus) {
            relBaseX = x;
            relBaseY = y;
            relBaseZ = z;
            TestFrustum = frus;
        }

        auto RawUnsafe() noexcept { return mData; }

        BoundingBox getBaseAABB();

        Frustum::ChunkBox getRelativeAABB();

        void Attach(std::shared_ptr<PmrBase> attachment) {
            mAttached.push_back(std::move(attachment));
        }

        void Detach(const std::shared_ptr<PmrBase>& attachment) {
            auto iter = std::find(mAttached.begin(), mAttached.end(), attachment);
            if (iter != mAttached.end()) mAttached.erase(iter);
        }

        void calcVisible() { visible = TestFrustum.FrustumTest(getRelativeAABB()); }
    };
}
