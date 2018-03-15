#pragma once

#include <CommonDefines.h>

interface IProcess {
    virtual ~IProcess() {}

    virtual bool Iteration() = 0;
    virtual bool IsTerminated() = 0;
};
