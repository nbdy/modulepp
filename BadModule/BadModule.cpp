//
// Created by nbdy on 07.09.21.
//

#include "BadModule.h"

BadModule::BadModule(): IModule("BaseModule") {

}

extern "C" BadModule* wrongExportFunctionName() {return new BadModule;}
