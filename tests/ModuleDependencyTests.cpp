//
// Created by nbdy on 28.05.22.
//

#include "gtest/gtest.h"
#include "modulepp.h"

TEST(ModuleDependency, equals) {
    ModuleDependency dep0("Dependency0");
    IModule dependentModule0(ModuleInformation("Module0"), std::vector{dep0});
    auto deps = dependentModule0.getModuleDependencies();
    EXPECT_TRUE(std::any_of(deps.begin(), deps.end(), [dep0](const ModuleDependency& i_Dependency) {
        return i_Dependency == dep0;
    }));
}