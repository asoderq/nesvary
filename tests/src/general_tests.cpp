#include "nesvary.hpp"
#include "gtest/gtest.h"

TEST(nesvary, version_is_0_0_1)

{
    ASSERT_STREQ("0.0.1", nesvary_version());
}
