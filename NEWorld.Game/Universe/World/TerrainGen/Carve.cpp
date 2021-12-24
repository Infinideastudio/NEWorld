namespace World::TerrainGen {
    /*
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
    */
}