#pragma once

#include <memory>
#include <vector>

#include <VirtualAddressSpace.h>
#include <ExecutableProvider.h>

class VAS16 : public IVirtualAddressSpace<Word> {
public:
    VAS16(IExecutableProvider* provider);

    virtual std::byte Read(Word pointer) override;
    virtual Word ReadWord(Word pointer) override;
    virtual Dword ReadDword(Word pointer) override;
    virtual void Write(Word pointer, Dword value) override;
    virtual void Write(Word pointer, Word value) override;
    virtual void Write(Word pointer, std::byte value) override;
    virtual const char* GetPhysical(Word pointer) override;
    virtual void DumpAllTo(std::ostream& output) override;

    void SetStackPtr(char* stack) { this->stack = reinterpret_cast<std::byte*>(stack); }

private:
    const IExecutableProvider* provider;
    const unsigned short stackOffset;
    const unsigned short stackSize;
    std::byte* stack;

    Word stackAddress(Word pointer) { return pointer - stackOffset; }
};
