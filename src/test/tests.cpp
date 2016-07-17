/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>


//***********Vec3***********//
#include <vec3.h>
TEST(Vec3, Basic)
{
    Vec3<int> v(1, 2, 3);
    EXPECT_EQ(v.x + v.y*v.z, 7);
}
TEST(Vec3, Length)
{
    Vec3<int> v(3, 6, 6);
    EXPECT_EQ(v.length(), 9);
}
TEST(Vec3, for_each)
{
    Vec3<int> v(3, 6, 6);
    v.for_each([](int& x,int y) { x+=y; }, 1);
    EXPECT_EQ(v.lengthSqr(), 114);
}

//***********Type***********//
#include <type.h>
TEST(Type, is_tests)
{
    EXPECT_TRUE(isDecimal("1.2"));
    EXPECT_TRUE(isDecimal("1."));
    EXPECT_TRUE(isDecimal("0.9"));
    EXPECT_TRUE(isDecimal("0.09"));
    EXPECT_TRUE(isDecimal("1"));
    EXPECT_TRUE(isDecimal(".9"));
    EXPECT_TRUE(isDecimal("+.9"));
    EXPECT_TRUE(isDecimal("-.9"));
    EXPECT_FALSE(isDecimal("1.2.3"));
    EXPECT_FALSE(isDecimal("1..0"));
    EXPECT_FALSE(isDecimal("1.2s"));
    EXPECT_FALSE(isDecimal("a.2s"));
    EXPECT_FALSE(isDecimal("asd"));
    EXPECT_FALSE(isDecimal(""));

    EXPECT_TRUE(isInteger("1"));
    EXPECT_TRUE(isInteger("123"));
    EXPECT_TRUE(isInteger("+3"));
    EXPECT_TRUE(isInteger("0"));
    EXPECT_TRUE(isInteger("-3"));
    EXPECT_FALSE(isInteger("1.2"));
    EXPECT_FALSE(isInteger("+1s"));
    EXPECT_FALSE(isInteger("90."));
    EXPECT_FALSE(isInteger("asd"));
    EXPECT_FALSE(isInteger(""));

    EXPECT_TRUE(isBoolean("true"));
    EXPECT_TRUE(isBoolean("false"));
    EXPECT_TRUE(isBoolean("1"));
    EXPECT_TRUE(isBoolean("0"));
    EXPECT_FALSE(isBoolean("ture"));
    EXPECT_FALSE(isBoolean("3"));
    EXPECT_FALSE(isBoolean("-1"));
    EXPECT_FALSE(isBoolean(""));

    EXPECT_TRUE(isString("\"str\""));
    EXPECT_TRUE(isString("\"\""));
    EXPECT_FALSE(isString("true"));
    EXPECT_FALSE(isString("3"));
    EXPECT_FALSE(isString("-1"));
    EXPECT_FALSE(isString(""));
}
TEST(Type, get_tests)
{
    EXPECT_EQ(getDecimal("1.2"), 1.2);
    EXPECT_EQ(getDecimal("1."),1);
    EXPECT_EQ(getDecimal("0.9"),0.9);
    EXPECT_EQ(getDecimal("0.09"),0.09);
    EXPECT_EQ(getDecimal("1"),1);
    EXPECT_EQ(getDecimal(".9"),0.9);
    EXPECT_EQ(getDecimal("+.9"),0.9);
    EXPECT_EQ(getDecimal("-.9"),-0.9);

    EXPECT_EQ(getInteger("1"),1);
    EXPECT_EQ(getInteger("123"),123);
    EXPECT_EQ(getInteger("+3"),3);
    EXPECT_EQ(getInteger("0"),0);
    EXPECT_EQ(getInteger("-3"),-3);

    EXPECT_EQ(getBoolean("true"),true);
    EXPECT_EQ(getBoolean("false"),false);
    EXPECT_EQ(getBoolean("1"),true);
    EXPECT_EQ(getBoolean("0"),false);

    EXPECT_EQ(getString("\"str\""),"str");
    EXPECT_EQ(getString("\"\""),"");
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
