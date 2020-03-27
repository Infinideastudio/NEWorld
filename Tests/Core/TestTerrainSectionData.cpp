#include <Game/World/Chunk/TerrainSectionData.h>
#include <gtest/gtest.h>

TEST(Core, TerrainSectionData) {
    auto getHash = [](int x, int y, int z) { return std::hash<size_t>()(x*32*32+y*32+z); };
    auto dataWrite = [&](TerrainSectionData& data, size_t maxSize) {
        for (int x = 0; x<32; x++)
            for (int y = 0; y<32; y++)
                for (int z = 0; z<32; z++) {
                    auto d = BlockData(getHash(x, y, z)%maxSize);
                    data.setBlock({x, y, z}, d);
                }
    };
    auto dataValidate = [&](TerrainSectionData& data, size_t maxSize) {
        for (int x = 0; x<32; x++)
            for (int y = 0; y<32; y++)
                for (int z = 0; z<32; z++) {
                    const auto l = data.getBlock({x, y, z});
                    const auto r = BlockData(getHash(x, y, z)%maxSize);
                    ASSERT_EQ(l, r);
                }
    };
    auto dataCheck = [&](TerrainSectionData& data, size_t maxSize) {
        dataWrite(data, maxSize);
        dataValidate(data, maxSize);
    };
    TerrainSectionData data1, data2(8), data3(4);

    //dataCheck(data1, data1.getPaletteCapacity());
    dataCheck(data2, 1ull << data2.getBitLength());
    dataCheck(data3, 1ull << data3.getBitLength());

    // Upsizing test
    data2.setBlock({0, 0, 0}, 1024);
    ASSERT_EQ(data2.getBitLength(), 32);
    dataValidate(data2, 8);

    data3.setBlock({0, 0, 0}, 1024);
    ASSERT_EQ(data3.getBitLength(), 8);
    dataValidate(data3, 4);

    dataCheck(data2, 2ull << 32u);
    dataCheck(data3, 2ull << 32u);
}