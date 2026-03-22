#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "apply_function.h"

TEST(ApplyFunctionTest, WorksInSingleThread) {
    std::vector<int> data = {1, 2, 3, 4, 5};

    ApplyFunction<int>(data, [](int& x) { x *= 2; }, 1);

    EXPECT_EQ(data, std::vector<int>({2, 4, 6, 8, 10}));
}

TEST(ApplyFunctionTest, WorksInMultipleThreads) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8};

    ApplyFunction<int>(data, [](int& x) { x += 10; }, 4);

    EXPECT_EQ(data, std::vector<int>({11, 12, 13, 14, 15, 16, 17, 18}));
}

TEST(ApplyFunctionTest, ThreadCountGreaterThanDataSize) {
    std::vector<int> data = {1, 2, 3};

    ApplyFunction<int>(data, [](int& x) { x = x * x; }, 10);

    EXPECT_EQ(data, std::vector<int>({1, 4, 9}));
}

TEST(ApplyFunctionTest, EmptyVector) {
    std::vector<int> data;

    ApplyFunction<int>(data, [](int& x) { x += 1; }, 4);

    EXPECT_TRUE(data.empty());
}

TEST(ApplyFunctionTest, OneElement) {
    std::vector<int> data = {42};

    ApplyFunction<int>(data, [](int& x) { x -= 2; }, 8);

    ASSERT_EQ(data.size(), 1u);
    EXPECT_EQ(data[0], 40);
}

TEST(ApplyFunctionTest, SupportsOtherTypes) {
    std::vector<std::string> data = {"a", "bb", "ccc"};

    ApplyFunction<std::string>(data, [](std::string& s) { s += "!"; }, 2);

    EXPECT_EQ(data, std::vector<std::string>({"a!", "bb!", "ccc!"}));
}

TEST(ApplyFunctionTest, InvalidThreadCountThrows) {
    std::vector<int> data = {1, 2, 3};

    EXPECT_THROW(
        ApplyFunction<int>(data, [](int& x) { x += 1; }, 0),
        std::invalid_argument
    );

    EXPECT_THROW(
        ApplyFunction<int>(data, [](int& x) { x += 1; }, -5),
        std::invalid_argument
    );
}