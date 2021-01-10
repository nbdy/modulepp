//
// Created by nbdy on 10.01.21.
//

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