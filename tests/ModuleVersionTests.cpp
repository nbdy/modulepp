//
// Created by nbdy on 28.05.22.
//

#include "gtest/gtest.h"
#include "modulepp.h"

TEST(ModuleVersion, equals) {
    ModuleVersion version0(0, 1, 0);
    ModuleVersion version1(0, 1, 0);
    ModuleVersion version2(1, 0, 0);
    EXPECT_EQ(version0, version1);
    EXPECT_NE(version0, version2);
}