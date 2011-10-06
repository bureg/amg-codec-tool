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
    getchar();
    exit(1);
}
