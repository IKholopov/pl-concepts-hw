#include <Instruction.h>

#include <assert.h>
#include <iostream>

#define PARSE(Name, Value)  case Value: \
                                   return new Name(provider, addressSpace);\
                                   break;

IInstruction* Parse(TInstructionValue value, IRegistryProvider& provider, IVirtualAddressSpace<Word>& addressSpace) {
    switch (value) {
    PARSE(AsciiString, ASCII)
    PARSE(Number, NUMBER)
    PARSE(Move, MOV)
    PARSE(MoveConst, MOV_CONST)
    PARSE(Push, PUSH)
    PARSE(PushConst, PUSH_CONST)
    PARSE(Pop, POP)
    PARSE(Add, ADD)
    PARSE(AddConst, ADD_CONST)
    PARSE(Sub, SUB)
    PARSE(SubConst, SUB_CONST)
    PARSE(Inc, INC)
    PARSE(Dec, DEC)
    PARSE(Cmp, CMP)
    PARSE(CmpConst, CMP_CONST)
    PARSE(Jeq, JEQ)
    PARSE(Call, CALL)
    PARSE(Printf, PRINTF)
    PARSE(Printn, PRINTN)
    PARSE(Scann, SCANN)
    default:
        assert(false);
        break;
    }
}

namespace {
void PushRegistryToStack(std::byte registerIndex, IRegistryProvider& provider,
                 IVirtualAddressSpace<Word>& addressSpace) {
    Word* registry = provider.GetRegistryAt(registerIndex);
    addressSpace.Write(*provider.GetRegistryAt(std::byte(StackPointerRegister)), *registry);
    (*provider.GetRegistryAt(std::byte(StackPointerRegister))) += sizeof(Word);
}

void PopFromStack(std::byte registerIndex, IRegistryProvider& provider,
                 IVirtualAddressSpace<Word>& addressSpace) {
    Word* registry = provider.GetRegistryAt(registerIndex);
    (*provider.GetRegistryAt(std::byte(StackPointerRegister))) -= sizeof(Word);
    *registry = addressSpace.ReadWord(*provider.GetRegistryAt(std::byte(StackPointerRegister)));
}

}

void AsciiString::Eval(Dword value) {
    assert(false);
}

void Number::Eval(Dword value) {
    assert(false);
}

void Move::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    std::byte to = std::byte(data[1]);
    std::byte from = std::byte(data[2]);
    assert(static_cast<TInstructionValue>(command) == MOV);
    *provider.GetRegistryAt(to) = *provider.GetRegistryAt(from);
}

void MoveConst::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    std::byte to = std::byte(data[1]);
    Word constValue = *reinterpret_cast<Word*>(data+2);
    assert(static_cast<TInstructionValue>(command) == MOV_CONST);
    *provider.GetRegistryAt(to) = constValue;
}

void Push::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == PUSH);
    unsigned char begin = data[1];
    unsigned char seqVariant = data[2];
    unsigned char end = data[3];
    switch (seqVariant) {
    case NONE:
        PushRegistryToStack(std::byte(begin), provider, addressSpace);
        break;
    case SEPARATE:
        PushRegistryToStack(std::byte(begin), provider, addressSpace);
        PushRegistryToStack(std::byte(end), provider, addressSpace);
        break;
    case SEQUENCE:
        assert(end >= begin);
        for(unsigned char i = begin; i <= end; ++i) {
            PushRegistryToStack(std::byte(i), provider, addressSpace);
        }
        break;
    default:
        assert(false);
    }
}

void PushConst::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    Word constValue = *reinterpret_cast<Word*>(data+1);
    assert(static_cast<TInstructionValue>(command) == PUSH_CONST);
    addressSpace.Write(*provider.GetRegistryAt(std::byte(StackPointerRegister)), constValue);
    (*provider.GetRegistryAt(std::byte(StackPointerRegister)))++;
}

void Pop::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == POP);
    unsigned char begin = data[1];
    unsigned char seqVariant = data[2];
    unsigned char end = data[3];
    switch (seqVariant) {
    case NONE:
        PopFromStack(std::byte(begin), provider, addressSpace);
        break;
    case SEPARATE:
        PopFromStack(std::byte(end), provider, addressSpace);
        PopFromStack(std::byte(begin), provider, addressSpace);
        break;
    case SEQUENCE:
        assert(end >= begin);
        for(unsigned char i = end; i >= begin; --i) {
            PopFromStack(std::byte(i), provider, addressSpace);
        }
        break;
    default:
        assert(false);
    }
}

void Add::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == ADD);
    std::byte first = std::byte(data[1]);
    std::byte second = std::byte(data[2]);
    *provider.GetRegistryAt(first) += *provider.GetRegistryAt(second);
}

void AddConst::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == ADD_CONST);
    std::byte first = std::byte(data[1]);
    Word second = *reinterpret_cast<Word*>(data + 2);
    *provider.GetRegistryAt(first) += second;
}

void Sub::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == SUB);
    std::byte first = std::byte(data[1]);
    std::byte second = std::byte(data[2]);
    *provider.GetRegistryAt(first) -= *provider.GetRegistryAt(second);
}

void SubConst::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == SUB_CONST);
    std::byte first = std::byte(data[1]);
    Word second = *reinterpret_cast<Word*>(data + 2);
    *provider.GetRegistryAt(first) -= second;
}

void Inc::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == INC);
    std::byte first = std::byte(data[1]);
    ++*provider.GetRegistryAt(first);
}

void Dec::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == DEC);
    std::byte first = std::byte(data[1]);
    --*provider.GetRegistryAt(first);
}

void Cmp::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == CMP);
    std::byte first = std::byte(data[1]);
    std::byte second = std::byte(data[2]);
    *provider.GetRegistryAt(std::byte(ResultRegister)) = *provider.GetRegistryAt(first) - *provider.GetRegistryAt(second);
}

void CmpConst::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == CMP_CONST);
    std::byte first = std::byte(data[1]);
    Word second = *reinterpret_cast<Word*>(data + 2);
    *provider.GetRegistryAt(std::byte(ResultRegister)) = *provider.GetRegistryAt(first) - second;
}

void Jeq::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    Word address = *reinterpret_cast<Word*>(data+1);
    assert(static_cast<TInstructionValue>(command) == JEQ);
    if(*provider.GetRegistryAt(std::byte(ResultRegister)) == 0) {
        *provider.GetRegistryAt(std::byte(ProcessCounterRegister)) = address - sizeof(Dword);
    }
}

void Call::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    Word address = *reinterpret_cast<Word*>(data+1);
    assert(static_cast<TInstructionValue>(command) == CALL);
    *provider.GetRegistryAt(std::byte(LinkRegister)) = *provider.GetRegistryAt(std::byte(ProcessCounterRegister));
    *provider.GetRegistryAt(std::byte(ProcessCounterRegister)) = address - sizeof(Dword);
}

void Printf::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == PRINTF);
    const char* str = addressSpace.GetPhysical(*provider.GetRegistryAt(std::byte(ResultRegister))+sizeof(Dword));
    std::cout << str;
}

void Printn::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == PRINTN);
    std::cout << *provider.GetRegistryAt(std::byte(ResultRegister)) << std::endl;
}

void Scann::Eval(Dword value) {
    char* data = reinterpret_cast<char*>(&value);
    std::byte command = std::byte(data[0]);
    assert(static_cast<TInstructionValue>(command) == SCANN);
    std::cin >> *provider.GetRegistryAt(std::byte(ResultRegister));
}
