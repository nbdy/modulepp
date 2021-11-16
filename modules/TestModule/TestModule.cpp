//
// Created by nbdy on 07.09.21.
//

#include "TestModule.h"
#include <iostream>

TestModule::TestModule() : IModule(ModuleInformation {"TestModule"}) {}

void TestModule::work() {
  std::cout << "aye" << std::endl;
  m_u32Counter += 1U;
}

F_CREATE(TestModule)
