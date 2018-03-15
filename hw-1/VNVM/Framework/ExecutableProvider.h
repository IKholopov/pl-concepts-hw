#pragma once

#include <CommonDefines.h>

interface IExecutableProvider {
    virtual ~IExecutableProvider() {}

    virtual const char* GetExecutable() const = 0;
    virtual int StackSize() const = 0;
    virtual int ExecutableSize() const = 0;
};
