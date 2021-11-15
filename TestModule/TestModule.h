//
// Created by nbdy on 07.09.21.
//

#ifndef MODULEPP_TESTMODULE_TESTMODULE_H_
#define MODULEPP_TESTMODULE_TESTMODULE_H_

#include "../modulepp.h"

class TestModule : public IModule {
 private:
  uint32_t m_u32Counter = 0U;

 public:
  TestModule();

  void work() override;

  [[nodiscard]] uint32_t getCounter() const {
    return m_u32Counter;
  };
};

#endif //MODULEPP_TESTMODULE_TESTMODULE_H_
