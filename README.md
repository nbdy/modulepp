# modulepp
[![CodeFactor](https://www.codefactor.io/repository/github/nbdy/modulepp/badge/master)](https://www.codefactor.io/repository/github/nbdy/modulepp/overview/master)
## features
- [X] shared object loading
  - [X] load single shared object
  - [X] load whole directory of shared objects
- [X] module interface / baseline

- [X] test code coverage is 97%
## example
```c++
// TestModule.h
#ifndef MODULEPP_TESTMODULE_TESTMODULE_H_
#define MODULEPP_TESTMODULE_TESTMODULE_H_

#include "modulepp.h"

class TestModule : public IModule {
  private:
    uint32_t m_u32Counter = 0;

  public:
    TestModule();

    void work() override;
  
    [[nodiscard]] uint32_t getCounter() const{
      return m_u32Counter;
    };
};

#endif //MODULEPP_TESTMODULE_TESTMODULE_H_
```
```c++
// TestModule.cpp
#include "TestModule.h"

TestModule::TestModule(): IModule("TestModule") {}

void TestModule::work() {
  m_u32Counter += 1;
}

F_CREATE(TestModule)
```
```c++
// main.cpp
#include <iostream>
#include "modulepp.h"

int main(int argc, char** argv) {
    std::string so = "/home/nbdy/CLionProjects/libmodulepp/cmake-build-debug/libtest_module.so";
    if(argc > 1) so = argv[1];
    auto* m = ModuleLoader::load(so, true);
    if(m == nullptr) std::cout << "could not load library" << std::endl;
    else m->start();
    m.join();
    return 0;
}
```