#include <VNProcess.h>

#include <Instruction.h>
#include <iostream>
#include <memory>

VNProcess::VNProcess(std::unique_ptr<IVirtualAddressSpace<unsigned short> >& space, std::unique_ptr<IRegistryProvider>& registers) :
    space(std::move(space)),
    registers(std::move(registers))
{
}

bool VNProcess::Iteration()
{
    if(IsTerminated()) {
        return false;
    }
    std::byte value = space->Read(*registers->GetRegistryAt(std::byte(ProcessCounterRegister)));
    std::unique_ptr<IInstruction> instruction(Parse(static_cast<TInstructionValue>(value), *registers, *space));
    instruction->Eval(space->ReadDword(*registers->GetRegistryAt(std::byte(ProcessCounterRegister))));
    if(IsTerminated()) {
        return false;
    }
    *registers->GetRegistryAt(std::byte(ProcessCounterRegister)) += sizeof(Dword);
    return true;
}

bool VNProcess::IsTerminated()
{
    if(*registers->GetRegistryAt(std::byte(ProcessCounterRegister)) == TerminalState) {
        return true;
    }
    return false;
}

void VNProcess::DumpStack(int n)
{
    for(int i = 0; i < n; ++i) {
        std::byte value = space->Read(*registers->GetRegistryAt(std::byte(StackPointerRegister)) - i);
        std::cout << std::hex << static_cast<unsigned int>(value);
    }
}

void VNProcess::DumpImage(std::string path)
{
    std::ofstream output(path, std::ofstream::out);
    space->DumpAllTo(output);
}

void VNProcess::DumpRegisters()
{
    for(int i = 0; i < StackPointerRegister; ++i) {
        std::cout << "r" << i << ": " << *registers->GetRegistryAt(std::byte(i)) << " ";
    }
    std::cout << "sp: " << *registers->GetRegistryAt(std::byte(StackPointerRegister)) << " ";
    std::cout << "lr: " << *registers->GetRegistryAt(std::byte(LinkRegister)) << " ";
    std::cout << "pc: " << *registers->GetRegistryAt(std::byte(ProcessCounterRegister)) << std::endl;
}
