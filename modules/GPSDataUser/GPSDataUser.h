//
// Created by nbdy on 16.11.21.
//

#ifndef MODULEPP_MODULES_GPSDATAUSER_GPSDATAUSER_H_
#define MODULEPP_MODULES_GPSDATAUSER_GPSDATAUSER_H_

#include "modulepp.h"

class GPSDataUser : public IModule {
public:
  GPSDataUser();
  void work() override;
};

#endif // MODULEPP_MODULES_GPSDATAUSER_GPSDATAUSER_H_
