#pragma once

#include <CommonDefines.h>
#include <cstddef>
#include <fstream>

template<typename Ptr>
interface IVirtualAddressSpace {
    virtual ~IVirtualAddressSpace() {}

    virtual std::byte Read(Ptr pointer) = 0;
    virtual Word ReadWord(Ptr pointer) = 0;
    virtual Dword ReadDword(Ptr pointer) = 0;
    virtual const char* GetPhysical(Ptr pointer) = 0;
    virtual void Write(Ptr pointer, Dword value) = 0;
    virtual void Write(Ptr pointer, Word value) = 0;
    virtual void Write(Ptr pointer, std::byte value) = 0;
    virtual void DumpAllTo(std::ostream& output) = 0;
};
