//
// Created by nbdy on 10.01.21.
//

#ifndef MODULEPP_TESTMODULE_H
#define MODULEPP_TESTMODULE_H

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

#endif //MODULEPP_TESTMODULE_H
