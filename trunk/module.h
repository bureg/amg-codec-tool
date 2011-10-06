#ifndef MODULE_H
#define MODULE_H

#include <QtGlobal>

class Module
{
public:
    Module();
    Module(char * moduleName);

protected:
    void fatalExit(char *reason);

private:
    static const size_t MAX_MODULE_NAME_LENGTH = 64;

    char moduleName[MAX_MODULE_NAME_LENGTH + 1];
};

#endif // MODULE_H
