#pragma once

#include <Process.h>
#include <VirtualAddressSpace.h>
#include <RegistryProvider.h>
#include <memory>

class VNProcess : public IProcess {
public:
    VNProcess(std::unique_ptr<IVirtualAddressSpace<Word>>& space, std::unique_ptr<IRegistryProvider>& registers);
    virtual bool Iteration() override;
    virtual bool IsTerminated() override;

    void DumpRegisters();
    void DumpStack(int n);
    void DumpImage(std::string path);

private:
    std::unique_ptr<IVirtualAddressSpace<Word>> space;
    std::unique_ptr<IRegistryProvider> registers;
};
