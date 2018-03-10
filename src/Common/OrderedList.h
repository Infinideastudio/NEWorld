// 
// Core: OrderedList.h
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once
#include <functional>

// POD ONLY!
template <class Tk, class Td, int size, template<class>class Compare = std::less>
class PodOrderedList {
public:
    PodOrderedList() noexcept : mSize(0), mComp() {}
    using ArrayType = std::array<std::pair<Tk, Td>, size>;
    using Iterator = typename ArrayType::iterator;
    using ConstIterator = typename ArrayType::const_iterator;
    Iterator begin() noexcept { return mList.begin(); }
    ConstIterator begin() const noexcept { return mList.begin(); }
    Iterator end() noexcept { return mList.begin() + mSize; }
    ConstIterator end() const noexcept { return mList.begin() + mSize; }

    void insert(Tk key, Td data) noexcept {
        auto first = 0, last = mSize - 1;
        while (first <= last) {
            int middle = (first + last) / 2;
            if (mComp(key, mList[middle].first))
                last = middle - 1;
            else
                first = middle + 1;
        }
        if (first <= mSize && first < size) {
            mSize = std::min(size, mSize + 1);
            for (auto j = size - 1; j > first; j--)
                mList[j] = mList[j - 1];
            mList[first] = std::pair<Tk, Td>(key, data);
        }
    }

    void clear() noexcept { mSize = 0; }
private:
    int mSize;
    ArrayType mList;
    Compare<Tk> mComp;
};
