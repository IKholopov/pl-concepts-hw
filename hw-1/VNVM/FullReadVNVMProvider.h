#pragma once

#include <ExecutableProvider.h>
#include <RegistryProvider.h>
#include <istream>
#include <vector>

class FullReadNVVMProvider : public IExecutableProvider, public IRegistryProvider
{
public:
    FullReadNVVMProvider(std::istream& inputStream);

    virtual const char* GetExecutable() const override { return values.data(); }
    virtual int StackSize() const override { return stackSize; }
    virtual int ExecutableSize() const override { return executableSize; }

    virtual Word* GetRegistryAt(std::byte i) override;
    virtual int GetRegistersCount() override { return registersCount; }

    char* GetStackPtr();

private:
    Word stackSize;
    Word registersCount;
    int executableSize;
    std::vector<char> values;
};
