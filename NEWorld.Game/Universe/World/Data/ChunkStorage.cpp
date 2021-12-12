#include "ChunkStorage.h"

namespace World::Data {
    ChunkStorage::ChunkStorage(int bits) noexcept
            :mT(CheckMode(bits)), mBit(bits) {
        switch (mT) {
        case 0:
            new(&mV.b4) BlocksSparse4();
            new(&mP.p4) BlockPalette4();
            return;
        case 1:
            new(&mV.b8) BlocksSparse8();
            new(&mP.p8) BlockPalette8(bits);
            return;
        case 2:
            new(&mV.b16) BlocksSparse16();
            new(&mP.p16) BlockPalette16();
            return;
        }
    }

    ChunkStorage::ChunkStorage(int bits, InitializeT) noexcept
            :mT(CheckMode(bits)), mBit(bits) {
        switch (mT) {
        case 0:
            new(&mV.b4) BlocksSparse4(Init);
            new(&mP.p4) BlockPalette4();
            return;
        case 1:
            new(&mV.b8) BlocksSparse8(Init);
            new(&mP.p8) BlockPalette8(bits);
            return;
        case 2:
            new(&mV.b16) BlocksSparse16(Init);
            new(&mP.p16) BlockPalette16();
            return;
        }
    }

    ChunkStorage::~ChunkStorage() noexcept {
        switch (mT) {
        case 0: (mV.b4.~BlocksSparse4(), mP.p4.~BlockPalette4());
            return;
        case 1: (mV.b8.~BlocksSparse8(), mP.p8.~BlockPalette8());
            return;
        case 2: (mV.b16.~BlocksSparse16(), mP.p16.~BlockPalette16());
            return;
        }
    }

    bool ChunkStorage::TrySet(const int index, const uintptr_t val) noexcept {
        switch (mT) {
        case 0: {
            auto id = mP.p4.fromVal(val);
            return (id != -1) && (mV.b4.Set(index, id), true);
        }
        case 1: {
            auto id = mP.p8.fromVal(val);
            return (id != -1) && (mV.b8.Set(index, id), true);
        }
        case 2: {
            auto id = mP.p16.fromVal(val);
            return (id != -1) && (mV.b16.Set(index, id), true);
        }
        }
        return false;
    }

    void ChunkStorage::Scale48() noexcept {
        BlocksSparse8 b8{Init};
        BlockPalette8 p8{mP.p4};
        for (int i = 0; i<4096; ++i) b8.Set(i, mV.b4.Get(i)); // mId is kept
        // close the 4-bit storage
        mV.b4.~BlocksSparse4();
        mP.p4.~BlockPalette4();
        // switch mode
        mBit = 5;
        mT = 1;
        // enable the 8-bit storage
        new(&mV.b8) BlocksSparse8(std::move(b8));
        new(&mP.p8) BlockPalette8(std::move(p8));
    }

    void ChunkStorage::Scale8H() noexcept {
        BlocksSparse16 b16{Init};
        BlockPalette16 p16{};
        for (int i = 0; i<4096; ++i) b16.Set(i, p16.fromVal(mP.p8.toVal(mV.b8.Get(i))));
        // close the 8-bit storage
        mV.b8.~BlocksSparse8();
        mP.p8.~BlockPalette8();
        // switch mode
        mBit = 9;
        mT = 2;
        // enable the 16-bit storage
        new(&mV.b16) BlocksSparse16(std::move(b16));
        new(&mP.p16) BlockPalette16(p16);
    }

    void ChunkStorage::UpScale() noexcept {
        switch (mT) {
        case 0: Scale48();
            break;
        case 1: ((mBit<8) ? Scale88() : Scale8H());
            break;
        }
    }

    void ChunkStorage::Set(const int index, const uintptr_t val) noexcept {
        if (!TrySet(index, val)) {
            UpScale();
            TrySet(index, val);
        }
    }

    void ChunkStorage::Scale88() noexcept {
        (mP.p8.UpScale(), ++mBit);
    }
}
