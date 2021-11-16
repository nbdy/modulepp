//
// Created by nbdy on 16.11.21.
//

#include <iostream>
#include "GPSDataUser.h"

GPSDataUser::GPSDataUser() : IModule(ModuleInformation {"GPSDataUser"}, {ModuleDependency {"GPS"}}) {}

void GPSDataUser::work() {
  auto data = m_DependencyMap["GPS"]->getSharedData();
  std::cout << "Lng: " << data["longitude"] << " | Lat: " << data["latitude"] << std::endl;
}

F_CREATE(GPSDataUser)
