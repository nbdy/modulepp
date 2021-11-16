//
// Created by nbdy on 07.09.21.
//

#include "BadModule.h"

BadModule::BadModule() : IModule(ModuleInformation {"BaseModule"}) {}

extern "C" BadModule* wrongExportFunctionName() { return new BadModule; }