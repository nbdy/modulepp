//
// Created by nbdy on 28.05.22.
//

#include "modulepp.h"

int main(int argc, char** argv) {
    std::string so = "libtest_module.so";
    if(argc > 1) so = argv[1];
    auto* m = ModuleLoader::load<IModule>(so, true);
    if(m == nullptr) std::cout << "could not load library" << std::endl;
    else m->start();
    m->join();
    return 0;
}
