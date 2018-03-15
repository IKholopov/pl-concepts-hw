#include <VNDisassembler.h>

#include <Instruction.h>
#include <assert.h>

VNDisassembler::VNDisassembler() :
    stackSize(-1),
    readSize(0),
    executableSize(-1)
{
}

static std::string RegisterToString(char reg) {
    assert(reg >= 0);
    assert(reg < RegsCount);
    switch (reg) {
    case ProcessCounterRegister:
        return "pc";
    case LinkRegister:
        return "lr";
    case StackPointerRegister:
        return "sp";
    default:
        return std::string("r") + std::to_string(static_cast<int>(reg));
        break;
    }
}

void VNDisassembler::Parse(std::istream& input, std::ostream& output)
{
    ReadFromInput(input, &stackSize, sizeof(Word));
    output << ".stack " << stackSize << std::endl;
    ReadFromInput(input, &executableSize, sizeof(Word));
    Word temp = 0;
    for(int i = 0; i < ProcessCounterRegister; ++i) {
        ReadFromInput(input, &temp, sizeof(Word));
    }
    ReadFromInput(input, &temp, sizeof(Word));
    output << ".entry " << temp << std::endl;
    TInstructionValue instruction;
    while(readSize < executableSize) {
        output << readSize << ":" << std::endl;
        ReadFromInput(input, &instruction, 1);
        switch (instruction) {
        case ASCII:
            parseAcii(input, output);
            break;
        case NUMBER:
            parseNumber(input, output);
            break;
        case MOV:
            parseMov(input, output);
            break;
        case MOV_CONST:
            parseMovConst(input, output);
            break;
        case PUSH:
            parsePush(input, output);
            break;
        case PUSH_CONST:
            parsePushConst(input, output);
            break;
        case POP:
            parsePop(input, output);
            break;
        case ADD:
            parseAdd(input, output);
            break;
        case ADD_CONST:
            parseAddConst(input, output);
            break;
        case SUB:
            parseSub(input, output);
            break;
        case SUB_CONST:
            parseSubConst(input, output);
            break;
        case INC:
            parseInc(input, output);
            break;
        case DEC:
            parseDec(input, output);
            break;
        case CMP:
            parseCmp(input, output);
            break;
        case CMP_CONST:
            parseCmpConst(input, output);
            break;
        case JEQ:
            parseJeq(input, output);
            break;
        case CALL:
            parseCall(input, output);
            break;
        case PRINTF:
            parsePrintf(input, output);
            break;
        case PRINTN:
            parsePrintn(input, output);
            break;
        case SCANN:
            parseScann(input, output);
            break;
        default:
            assert(false);
            break;
        }
    }

}

void VNDisassembler::parseAcii(std::istream& input, std::ostream& output)
{
    Word size;
    char tmp;
    ReadFromInput(input, &size, sizeof(Word));
    ReadFromInput(input, &tmp, 1);
    char buffer[size+2];
    ReadFromInput(input, buffer, size+2);
    std::string str = buffer;
    output << "\t.ascii " << "\"" << str << "\"" << std::endl;
}

void VNDisassembler::parseNumber(std::istream& input, std::ostream& output)
{
    Word val;
    ReadFromInput(input, &val, sizeof(Word));
    output << "\t.number " << val << std::endl;
}

void VNDisassembler::parseMov(std::istream& input, std::ostream& output)
{
    char reg1, reg2;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &reg2, 1);
    char tmp;
    ReadFromInput(input, &tmp, 1);
    output << "\tMOV " << RegisterToString(reg1) << ", " << RegisterToString(reg2) << std::endl;
}

void VNDisassembler::parseMovConst(std::istream& input, std::ostream& output)
{
    char reg1;
    ReadFromInput(input, &reg1, 1);
    Word val;
    ReadFromInput(input, &val, sizeof(Word));
    output << "\tMOV " << RegisterToString(reg1) << ", #" << val << std::endl;
}

void VNDisassembler::parsePush(std::istream& input, std::ostream& output)
{
    char reg1;
    ReadFromInput(input, &reg1, 1);
    TSeqVariant variant;
    ReadFromInput(input, &variant, 1);
    char tmp, reg2;
    switch (variant) {
    case NONE:
        ReadFromInput(input, &tmp, 1);
        output << "\tPUSH { " << RegisterToString(reg1) << " }" << std::endl;
        break;
    case SEPARATE:
        ReadFromInput(input, &reg2, 1);
        output << "\tPUSH { " << RegisterToString(reg1) << ", " << RegisterToString(reg2) << " }" << std::endl;
        break;
    case SEQUENCE:
        ReadFromInput(input, &reg2, 1);
        output << "\tPUSH { " << RegisterToString(reg1) << " - " << RegisterToString(reg2) << " }" << std::endl;
        break;
    default:
        assert(false);
        break;
    }
}

void VNDisassembler::parsePushConst(std::istream& input, std::ostream& output)
{
    Word val;
    char tmp;
    ReadFromInput(input, &val, sizeof(Word));
    ReadFromInput(input, &tmp, 1);
    output << "\t PUSH #" << val << std::endl;
}

void VNDisassembler::parsePop(std::istream& input, std::ostream& output)
{
    char reg1;
    ReadFromInput(input, &reg1, 1);
    TSeqVariant variant;
    ReadFromInput(input, &variant, 1);
    char tmp, reg2;
    switch (variant) {
    case NONE:
        ReadFromInput(input, &tmp, 1);
        output << "\tPOP { " << RegisterToString(reg1) << " }" << std::endl;
        break;
    case SEPARATE:
        ReadFromInput(input, &reg2, 1);
        output << "\tPOP { " << RegisterToString(reg1) << ", " << RegisterToString(reg2) << " }" << std::endl;
        break;
    case SEQUENCE:
        ReadFromInput(input, &reg2, 1);
        output << "\tPOP { " << RegisterToString(reg1) << " - " << RegisterToString(reg2) << " }" << std::endl;
        break;
    default:
        assert(false);
        break;
    }
}

void VNDisassembler::parseAdd(std::istream& input, std::ostream& output)
{
    char reg1, reg2, tmp;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &reg2, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tADD " << RegisterToString(reg1) << ", " << RegisterToString(reg2) << std::endl;
}

void VNDisassembler::parseAddConst(std::istream& input, std::ostream& output)
{
    char reg1;
    Word val;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &val, sizeof(Word));
    output << "\tADD " << RegisterToString(reg1) << ", #" << val << std::endl;
}

void VNDisassembler::parseSub(std::istream& input, std::ostream& output)
{
    char reg1, reg2, tmp;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &reg2, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tSUB " << RegisterToString(reg1) << ", " << RegisterToString(reg2) << std::endl;
}

void VNDisassembler::parseSubConst(std::istream& input, std::ostream& output)
{
    char reg1;
    Word val;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &val, sizeof(Word));
    output << "\tSUB " << RegisterToString(reg1) << ", #" << val << std::endl;
}

void VNDisassembler::parseInc(std::istream& input, std::ostream& output)
{
    char reg1, tmp;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tINC" << RegisterToString(reg1) << std::endl;
}

void VNDisassembler::parseDec(std::istream& input, std::ostream& output)
{
    char reg1, tmp;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tDEC" << RegisterToString(reg1) << std::endl;
}

void VNDisassembler::parseCmp(std::istream& input, std::ostream& output)
{
    char reg1, reg2, tmp;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &reg2, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tCMP " << RegisterToString(reg1) << ", " << RegisterToString(reg2) << std::endl;
}

void VNDisassembler::parseCmpConst(std::istream& input, std::ostream& output)
{
    char reg1;
    Word val;
    ReadFromInput(input, &reg1, 1);
    ReadFromInput(input, &val, sizeof(Word));
    output << "\tCMP " << RegisterToString(reg1) << ", #" << val << std::endl;
}

void VNDisassembler::parseJeq(std::istream& input, std::ostream& output)
{
    char tmp;
    Word address;
    ReadFromInput(input, &address, sizeof(Word));
    ReadFromInput(input, &tmp, 1);
    output << "\tJEQ " << address << std::endl;
}

void VNDisassembler::parseCall(std::istream& input, std::ostream& output)
{
    char tmp;
    Word address;
    ReadFromInput(input, &address, sizeof(Word));
    ReadFromInput(input, &tmp, 1);
    output << "\tBL " << address << std::endl;
}

void VNDisassembler::parsePrintf(std::istream& input, std::ostream& output)
{
    char tmp;
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tPRINTF" << std::endl;
}

void VNDisassembler::parsePrintn(std::istream& input, std::ostream& output)
{
    char tmp;
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tPRINTN" << std::endl;
}

void VNDisassembler::parseScann(std::istream& input, std::ostream& output)
{
    char tmp;
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    ReadFromInput(input, &tmp, 1);
    output << "\tSCANN" << std::endl;
}
