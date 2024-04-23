#include "better_cuckoo_hash_table.cpp"
#include "cuckoo_hash_table.cpp"
#include "gtest/gtest.h"
#include "spdlog/sinks/stdout_color_sinks.h"

TEST(CuckooHashTest, TinyInsertAndFindTest) {
    auto logger = spdlog::stderr_color_mt("tiny_insert_and_find_test");
    logger->set_level(spdlog::level::debug);

    CuckooHashTable<int, std::string> hash_table(10);

    ASSERT_TRUE(hash_table.insert(1, "one"));
    ASSERT_TRUE(hash_table.insert(2, "two"));
    logger->debug("Inserted 1 and 2");

    EXPECT_EQ(hash_table.find(1), "one");
    EXPECT_EQ(hash_table.find(2), "two");
    logger->debug("Found 1 and 2");
}

TEST(CuckooHashTest, InsertAndFindTest) {
    auto logger = spdlog::stderr_color_mt("insert_and_find_test");
    logger->set_level(spdlog::level::debug);

    CuckooHashTable<int, std::string> hash_table(10);

    for (int i = 0; i < 21; ++i) {
        ASSERT_TRUE(hash_table.insert(i, std::to_string(i)));
    }
    logger->debug("Inserted 21 elements");

    for (int i = 0; i < 21; ++i) {
        EXPECT_EQ(hash_table.find(i), std::to_string(i));
    }
    logger->debug("Found 21 elements");
}

TEST(CuckooHashTest, DeletionTest) {
    auto logger = spdlog::stderr_color_mt("deletion_test");
    logger->set_level(spdlog::level::debug);

    CuckooHashTable<int, std::string> hash_table(10);

    for (int i = 0; i < 21; ++i) {
        ASSERT_TRUE(hash_table.insert(i, std::to_string(i)));
    }
    logger->debug("Inserted 21 elements");

    ASSERT_TRUE(hash_table.remove(10));             // 删除其中一个元素
    EXPECT_EQ(hash_table.find(10), std::string{});  // 确保元素被删除
    logger->debug("Deleted element 10");
}

TEST(CuckooHashTest, PerformanceTest) {
    auto logger = spdlog::stderr_color_mt("performance_test");
    logger->set_level(spdlog::level::debug);

    CuckooHashTable<int, std::string> hash_table(300000);

    // 测试插入操作的性能
    auto startInsert = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; ++i) {
        ASSERT_TRUE(hash_table.insert(i, std::to_string(i)));
    }
    auto endInsert = std::chrono::high_resolution_clock::now();
    auto durationInsert = std::chrono::duration_cast<std::chrono::milliseconds>(
                              endInsert - startInsert)
                              .count();
    logger->debug("Inserted 1000000 elements in {} milliseconds",
                  durationInsert);

    // 测试查找操作的性能
    auto startFind = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; ++i) {
        EXPECT_EQ(hash_table.find(i), std::to_string(i));
    }
    auto endFind = std::chrono::high_resolution_clock::now();
    auto durationFind = std::chrono::duration_cast<std::chrono::milliseconds>(
                            endFind - startFind)
                            .count();
    logger->debug("Found 1000000 elements in {} milliseconds", durationFind);

    float spaceUsage =
        static_cast<float>(1000000) / hash_table.get_table_size();
    logger->info("Space usage: {}", spaceUsage);
}
TEST(CuckooHashTest, TinyBetterInsertAndFindTest) {
    auto logger = spdlog::stderr_color_mt("tiny_better_insert_and_find_test");
    logger->set_level(spdlog::level::debug);

    BetterCuckooHashTable<int, std::string> hash_table(10);

    ASSERT_TRUE(hash_table.insert(1, "one"));
    ASSERT_TRUE(hash_table.insert(2, "two"));
    logger->debug("Inserted 1 and 2");

    EXPECT_EQ(hash_table.find(1), "one");
    EXPECT_EQ(hash_table.find(2), "two");
    logger->debug("Found 1 and 2");
}

TEST(CuckooHashTest, BetterInsertAndFindTest) {
    auto logger = spdlog::stderr_color_mt("insert_and_find_test");
    logger->set_level(spdlog::level::debug);

    BetterCuckooHashTable<int, std::string> hash_table(10);

    for (int i = 0; i < 100; ++i) {
        ASSERT_TRUE(hash_table.insert(i, std::to_string(i)));
    }
    logger->debug("Inserted 21 elements");

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(hash_table.find(i), std::to_string(i));
    }
    logger->debug("Found 21 elements");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
