/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "module.h"

#include <QString>

Module::Module()
{
    qstrncpy(moduleName, "Undefined", qstrlen("Undefined"));
}

Module::Module(char * _moduleName)
{
    size_t lengh = strlen(_moduleName);
    qstrncpy(moduleName,
             _moduleName,
             (lengh > MAX_MODULE_NAME_LENGTH) ? (MAX_MODULE_NAME_LENGTH) : (lengh));
}

void Module::fatalExit(char *reason)
{
    fprintf(stderr, "Module [%s] operation abort! Reason: [%s]\n", moduleName, reason);
    exit(1);
}
