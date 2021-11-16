//
// Created by nbdy on 16.11.21.
//

#include "ModuleWithDependency.h"

ModuleWithDependency::ModuleWithDependency(): IModule(ModuleInformation {"ModuleWithDependency"}, {ModuleInformation {"TestModule"}}) {}

F_CREATE(ModuleWithDependency)
