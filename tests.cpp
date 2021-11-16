//
// Created by nbdy on 10.01.21.
//

#include "gtest/gtest.h"
#include "modulepp.h"
#include "modules/TestModule/TestModule.h"
#include "modules/BadModule/BadModule.h"

#define TEST_LIBRARY_PATH "modules/TestModule/libTestModule.so"
#define NON_EXISTENT_LIBRARY_PATH "/tmp/thislibraryprobablydoesnotexist.so"
#define BAD_MODULE_LIBRARY_PATH "libtest_badmodule.so"

TEST(modulepp, LoadTestLibrary) {
  EXPECT_TRUE(std::filesystem::exists(TEST_LIBRARY_PATH));
  auto* m = ModuleLoader::loadModule<TestModule>(TEST_LIBRARY_PATH, true);
  EXPECT_NE(m, nullptr); // could the module be loaded?
  EXPECT_TRUE(m->start());
  EXPECT_TRUE(m->isRunning()); // was the module started?
  EXPECT_FALSE(m->start()); // check that we cannot start the module again
  EXPECT_EQ(m->getCycleTime(), 500); // is the cycle time 500 (the default)?
  m->setCycleTime(800U); // set a new cycle time
  EXPECT_EQ(m->getCycleTime(), 800); // is the cycle time 800 now?
  std::this_thread::sleep_for(std::chrono::milliseconds(1600)); // sleep for a bit
  EXPECT_TRUE(m->isRunning()); // check if we are still running
  m->stop(); // check if stopping works
  EXPECT_FALSE(m->isEnabled()); // check that we are not running anymore
  EXPECT_FALSE(m->hasError()); // we should not have any errors
  EXPECT_EQ(m->getError(), ""); // the error string should also be empty
  EXPECT_EQ(m->getCounter(), 2);
  EXPECT_EQ(m->getInformation().getName(), "TestModule");
}

TEST(modulepp, LoadNotExistentLibrary) {
  EXPECT_FALSE(std::filesystem::exists(NON_EXISTENT_LIBRARY_PATH));
  auto* m = ModuleLoader::loadModule<TestModule>(NON_EXISTENT_LIBRARY_PATH, true);
  EXPECT_EQ(m, nullptr);
}

TEST(modulepp, LoadBadModule) {
  EXPECT_FALSE(std::filesystem::exists(BAD_MODULE_LIBRARY_PATH));
  auto* m = ModuleLoader::loadModule<BadModule>(BAD_MODULE_LIBRARY_PATH, true);
  EXPECT_EQ(m, nullptr);
}

TEST(modulepp, LoadDirectory) {
  auto cwd = std::filesystem::current_path() / "modules" / "TestModule";
  auto modules = ModuleLoader::loadDirectory<TestModule>(cwd, true);
/*
  for(auto module : modules) {
    if(module != nullptr) {
      std::cout << module->getInformation().getName() << std::endl;
    }
  }
  */
  EXPECT_EQ(modules.size(), 1);
  EXPECT_EQ(modules[0]->getInformation().getName(), "TestModule");
}

TEST(modulepp, ModuleManager) {
  ModuleManager mm("modules/", true, true);
  mm.start();
  std::this_thread::sleep_for(Milliseconds(2000));
  mm.stop();
}
