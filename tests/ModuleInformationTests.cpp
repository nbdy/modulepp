//
// Created by nbdy on 28.05.22.
//

#include "gtest/gtest.h"
#include "modulepp.h"

TEST(ModuleInformation, equals) {
    ModuleInformation info0("Module0");
    ModuleInformation info1("Module0", ModuleVersion(0, 1, 0));
    ModuleInformation info2("Module1");
    ModuleInformation info3("Module1", ModuleVersion(1, 0, 0));
    EXPECT_EQ(info0, info1);
    EXPECT_NE(info0, info2);
    EXPECT_NE(info0, info3);
    EXPECT_NE(info1, info3);
    EXPECT_NE(info2, info3);
}