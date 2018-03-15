#pragma once

#include <CommonDefines.h>
#include <cstddef>

interface IRegistryProvider {
    virtual ~IRegistryProvider() {}
    virtual Word* GetRegistryAt(std::byte i) = 0;
    virtual int GetRegistersCount() = 0;
};
