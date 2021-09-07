//
// Created by nbdy on 07.09.21.
//

#include "TestModule.h"

TestModule::TestModule() : IModule("TestModule") {}

void TestModule::work() {
  m_u32Counter += 1;
}

F_CREATE(TestModule)
