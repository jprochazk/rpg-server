

#include <gtest/gtest.h>

#include "endian_test.h"
#include "byte_buffer_test.h"
#include "sequence_test.h"
#include "metrics_test.h"

/// TODO: write integration tests for networking code using a lightweight boost.beast client

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}