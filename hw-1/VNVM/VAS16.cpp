#include <VAS16.h>
#include <assert.h>

VAS16::VAS16(IExecutableProvider* provider) :
    provider(provider),
    stackOffset(provider->ExecutableSize()),
    stackSize(provider->StackSize()),
    stack(0)
{
    assert(stackOffset > 0);
    assert(provider->ExecutableSize() < std::numeric_limits<Word>::max());
    assert(provider->StackSize() < std::numeric_limits<Word>::max());
}

std::byte VAS16::Read(Word pointer)
{
    if(pointer < stackOffset) {
        return std::byte(*(provider->GetExecutable() + pointer));
    }
    assert(pointer < stackOffset + stackSize);
    return stack[stackAddress(pointer)];
}

unsigned short VAS16::ReadWord(unsigned short pointer)
{
    Word val = 0;
    if(pointer < stackOffset) {
        for(int i = 0; i < sizeof(Word); ++i) {
            *(reinterpret_cast<char*>(&val)+i) = *(provider->GetExecutable() + pointer+i);
        }
        return val;
    }
    assert(pointer < stackOffset + stackSize);
    for(int i = 0; i < sizeof(Word); ++i) {
        *(reinterpret_cast<std::byte*>(&val)+i) = stack[stackAddress(pointer+i)];
    }
    return val;
}

unsigned int VAS16::ReadDword(unsigned short pointer)
{
    Dword val = 0;
    if(pointer < stackOffset) {
        for(int i = 0; i < sizeof(Dword); ++i) {
            *(reinterpret_cast<char*>(&val) + i) = *(provider->GetExecutable() + pointer + i);
        }
        return val;
    }
    assert(pointer < stackOffset + stackSize);
    for(int i = 0; i < sizeof(Dword); ++i) {
        *(reinterpret_cast<std::byte*>(&val)+i) = stack[stackAddress(pointer+i)];
    }
    return val;
}

void VAS16::Write(Word pointer, Dword value)
{
    assert(pointer >= stackOffset && pointer < stackOffset + stackSize - sizeof(Dword) + 1);
    Dword* address = reinterpret_cast<Dword*>(stack+stackAddress(pointer));
    *address = value;
}

void VAS16::Write(Word pointer, Word value)
{
    assert(pointer >= stackOffset && pointer < stackOffset + stackSize - sizeof(Word) + 1);
    Word* address = reinterpret_cast<Word*>(stack+stackAddress(pointer));
    *address = value;
}

void VAS16::Write(unsigned short pointer, std::byte value)
{
    assert(pointer >= stackOffset && pointer < stackOffset + stackSize);
    std::byte* address = stack+stackAddress(pointer);
    *address = value;
}

const char* VAS16::GetPhysical(unsigned short pointer)
{
    if(pointer < stackOffset) {
        return provider->GetExecutable() + pointer;
    }
    assert(pointer < stackOffset + stackSize);
    return reinterpret_cast<char*>(stack + stackAddress(pointer));
}

void VAS16::DumpAllTo(std::ostream& output)
{
    for(int i = 0; i < stackOffset + stackSize; ++i) {
        char val = static_cast<char>(Read(i));
        output.write(&val, 1);
    }
}

