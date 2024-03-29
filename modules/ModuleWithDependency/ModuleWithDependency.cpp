//
// Created by nbdy on 16.11.21.
//

#include <iostream>

#include "ModuleWithDependency.h"

#include "../TestModule/TestModule.h"

ModuleWithDependency::ModuleWithDependency(): IModule(ModuleInformation {"ModuleWithDependency"}, {ModuleDependency {"TestModule"}}) {}

void ModuleWithDependency::work() {
  std::cout << ((TestModule*) m_DependencyMap["TestModule"])->getCounter() << std::endl;
}

F_CREATE(ModuleWithDependency)
