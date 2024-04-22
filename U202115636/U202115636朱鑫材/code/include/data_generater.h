//
// Created by Pygon on 2024/4/10.
//

#include <cstdint>
#include <random>

static std::random_device rd;
static std::mt19937 gen(202115636);
static std::uniform_int_distribution<uint32_t> dis{std::numeric_limits<uint32_t>::min(),
                                                   std::numeric_limits<uint32_t>::max()};

class DataGenerater {
public:
    static uint32_t generate();
};

