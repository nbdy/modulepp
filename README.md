# modulepp
## features
- [X] shared object loading
- [X] module interface / baseline
## example
`look at tests/test.cpp and tests/TestModule.cpp`
```c++
// TestModule.cpp
#include <iostream>
#include "../modulepp.h"

class TestModule : public Module {
public:
    TestModule() = default;

void work() override {
    std::cout << "TestModule" << std::endl;
        this->stop();
    }
};

F_CREATE(TestModule);
```
```c++
// main.cpp
#include <iostream>
#include "../modulepp.h"

int main(int argc, char** argv) {
    std::string so = "/home/nbdy/CLionProjects/libmodulepp/cmake-build-debug/libtest_module.so";
    if(argc > 1) so = argv[1];
    auto* m = ModuleLoader::load(so, true);
    if(m == nullptr) std::cout << "could not load library" << std::endl;
    else m->start();
    while(m->isRunning()) {}
    return 0;
}
```