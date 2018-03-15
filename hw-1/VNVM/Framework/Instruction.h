#pragma once

#include <cstddef>

#include <CommonDefines.h>
#include <RegistryProvider.h>
#include <VirtualAddressSpace.h>

#define INSTRUCTION(Name, Value) class Name : public Instruction<Value> {\
public:\
    Name(IRegistryProvider& provider, IVirtualAddressSpace<Word>& addressSpace) : provider(provider), addressSpace(addressSpace) {}\
    virtual void Eval(Dword value) override; \
private:\
    IRegistryProvider& provider;\
    IVirtualAddressSpace<Word>& addressSpace;\
};

enum TRegisters : unsigned char {
    ResultRegister = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    StackPointerRegister,
    LinkRegister,
    ProcessCounterRegister,
    RegsCount
};

enum TInstructionValue : char {
    ASCII = 1,
    NUMBER,
    MOV,
    MOV_CONST,
    PUSH,
    PUSH_CONST,
    POP,
    ADD,
    ADD_CONST,
    SUB,
    SUB_CONST,
    INC,
    DEC,
    CMP,
    CMP_CONST,
    JEQ,
    CALL,
    PRINTF,
    PRINTN,
    SCANN,
    SET_COUNT
};

static const Word TerminalState = 0;

enum TSeqVariant : char {
    NONE,
    SEPARATE,
    SEQUENCE
};

interface IInstruction {
    virtual ~IInstruction() {}
    virtual TInstructionValue GetValue() const = 0;
    virtual void Eval(Dword value) = 0;
};

template<TInstructionValue Value>
class Instruction : public IInstruction {
public:
    virtual ~Instruction(){}
    virtual TInstructionValue GetValue() const override { return Value; }
};

IInstruction* Parse(TInstructionValue value, IRegistryProvider &provider, IVirtualAddressSpace<Word>& addressSpace);

INSTRUCTION(AsciiString, ASCII)
INSTRUCTION(Number, NUMBER)
INSTRUCTION(Move, MOV)
INSTRUCTION(MoveConst, MOV_CONST)
INSTRUCTION(Push, PUSH)
INSTRUCTION(PushConst, PUSH_CONST)
INSTRUCTION(Pop, POP)
INSTRUCTION(Add, ADD)
INSTRUCTION(AddConst, ADD_CONST)
INSTRUCTION(Sub, SUB)
INSTRUCTION(SubConst, SUB_CONST)
INSTRUCTION(Inc, INC)
INSTRUCTION(Dec, DEC)
INSTRUCTION(Cmp, CMP)
INSTRUCTION(CmpConst, CMP_CONST)
INSTRUCTION(Jeq, JEQ)
INSTRUCTION(Call, CALL)
INSTRUCTION(Printf, PRINTF)
INSTRUCTION(Printn, PRINTN)
INSTRUCTION(Scann, SCANN)
