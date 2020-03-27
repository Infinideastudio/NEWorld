#include <Game/World/Chunk/TerrainSectionData.h>
#include <gtest/gtest.h>

TEST(Core, TerrainSectionData) {
    auto getHash = [](int x, int y, int z) { return std::hash<size_t>()(x*32*32+y*32+z); };
    auto dataWrite = [&](TerrainSectionData& data, size_t maxSize) {
        for (int x = 0; x<32; x++)
            for (int y = 0; y<32; y++)
                for (int z = 0; z<32; z++) {
                    auto d = Game::World::BlockData(getHash(x, y, z)%maxSize);
                    data.setBlock({x, y, z}, d);
                }
    };
    auto dataValidate = [&](TerrainSectionData& data, size_t maxSize) {
        for (int x = 0; x<32; x++)
            for (int y = 0; y<32; y++)
                for (int z = 0; z<32; z++) {
                    const auto l = data.getBlock({x, y, z});
                    const auto r = Game::World::BlockData(getHash(x, y, z)%maxSize);
                    ASSERT_EQ(l, r);
                }
    };
    auto dataCheck = [&](TerrainSectionData& data, size_t maxSize) {
        dataWrite(data, maxSize);
        dataValidate(data, maxSize);
    };
    TerrainSectionData data1, data2(8), data3(4);
    ASSERT_EQ(data2.getBitLength(), 8);
    ASSERT_EQ(data3.getBitLength(), 4);

    dataCheck(data1, std::numeric_limits<size_t>::max());
    dataCheck(data2, 1ull << 8);
    dataCheck(data3, 1ull << 4);

    // Upsizing test
    auto oldBlock = data2.getBlock({ 0,0,0 });
    data2.setBlock({ 0, 0, 0 }, Game::World::BlockData(1024));
    data2.setBlock({ 0, 0, 0 }, oldBlock);
    ASSERT_EQ(data2.getBitLength(), 32);
    dataValidate(data2, 1 << 8);

    oldBlock = data3.getBlock({ 0,0,0 });
    data3.setBlock({ 0, 0, 0 }, Game::World::BlockData(1024));
    data3.setBlock({ 0, 0, 0 }, oldBlock);
    ASSERT_EQ(data3.getBitLength(), 8);
    dataValidate(data3, 1 << 4);

    // Optimize test
    data1.optimize();
    ASSERT_EQ(data1.getBitLength(), 32);
    dataValidate(data1, std::numeric_limits<size_t>::max());

    data2.optimize();
    ASSERT_EQ(data2.getBitLength(), 8);
    dataValidate(data2, 1 << 8);

    data3.optimize();
    ASSERT_EQ(data3.getBitLength(), 4);
    dataValidate(data3, 1 << 4);

    dataCheck(data2, std::numeric_limits<size_t>::max());
    dataCheck(data3, std::numeric_limits<size_t>::max());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
