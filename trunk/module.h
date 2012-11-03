/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef MODULE_H
#define MODULE_H

#include <assert.h>

#include <QtGlobal>

class Module
{
public:
    Module();
    Module(const char * moduleName);

protected:
    void fatalExit(const char *reason);

private:
    static const size_t MAX_MODULE_NAME_LENGTH = 64;

    char moduleName[MAX_MODULE_NAME_LENGTH + 1];
};

#endif // MODULE_H
