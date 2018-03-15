#include <VNAssembler.h>

#include <Instruction.h>
#include <assert.h>
#include <algorithm>

static const char PaddingValue = -1;

namespace {

std::string StripFromSpaces(std::string str) {
    std::string substr = str.substr(str.find_first_not_of(" \t"));
    if(substr.find_first_of(" \t\n") != std::string::npos) {
        substr = substr.substr(0, substr.find_first_of(" \t\n"));
    }
    return substr;
}

bool IsNumber(std::string arg) {
    for(auto it = arg.begin(); it != arg.end(); ++it) {
        if((*it) < '0' || (*it) > '9') {
            return false;
        }
    }
    return true;
}

bool IsRegister(std::string arg) {
    if(arg.compare("pc") == 0 || arg.compare("lr") == 0 || arg.compare("ap") == 0 ||
            (arg[0] == 'r' && IsNumber(arg.substr(1)) && std::stoi(arg.substr(1)) < RegsCount) ){
        return true;
    }
    return false;
}

TRegisters ParseRegister(std::string arg) {
    if(arg.compare("pc") == 0) {
        return ProcessCounterRegister;
    } else if(arg.compare("lr") == 0) {
        return LinkRegister;
    } else if(arg.compare("sp") == 0) {
        return StackPointerRegister;
    } else if(arg[0] == 'r') {
        TRegisters id = static_cast<TRegisters>(std::stoi(arg.substr(1)));
        assert(id < RegsCount);
        return id;
    } else {
        assert(false);
    }
}

}

VNAssembler::VNAssembler() :
    stackSize(-1),
    entryAddress(-1),
    executableSize(-1)
{
}

void VNAssembler::Parse(std::ifstream& input, std::ofstream& output)
{
    stackSize = -1;
    entryAddress = -1;
    executableSize = 0;
    tempBuffer.clear();
    while(!input.eof()) {
        std::string line;
        std::getline(input, line);
        if(line.size() == 0) {
            continue;
        }
        std::transform(line .begin(), line.end(), line.begin(), ::tolower);
        if(line.substr(line.find_first_not_of(" \t\n"), 1)[0] == '.') {
            parseDotEntry(line.substr(line.find_first_not_of(" \t\n"), line.size() - line.find_first_not_of(" \t\n")));
        } else if(line.size() > 0 && line[line.size() - 1] == ':') {
            parseLabel(line);
        } else {
            parseInstruction(line.substr(line.find_first_not_of(" \t\n"), line.size() - line.find_first_not_of(" \t\n")));
        }
    }
    for(auto it = labelsToFill.begin(); it != labelsToFill.end(); ++it) {
        for(auto val: it->second) {
            std::string value = it->first;
            *reinterpret_cast<Word*>(&tempBuffer[val]) = labelsToAddress.at(value) + RegsCount * sizeof(Word)+ sizeof(Word)*2;
        }
    }
    entryAddress = labelsToAddress.at(entryLabel) + RegsCount * sizeof(Word) + sizeof(Word)*2;
    executableSize += RegsCount * sizeof(Word) + sizeof(Word)*2;
    output.write(reinterpret_cast<char*>(&stackSize), sizeof(Word));
    output.write(reinterpret_cast<char*>(&executableSize), sizeof(Word));
    Word temp = 0;
    for(int i = 0; i < StackPointerRegister; ++i) {
        output.write(reinterpret_cast<char*>(&temp), sizeof(Word));
    }
    output.write(reinterpret_cast<char*>(&executableSize), sizeof(Word)); //SP
    output.write(reinterpret_cast<const char*>(&TerminalState), sizeof(Word)); //LR
    output.write(reinterpret_cast<char*>(&entryAddress), sizeof(Word)); //PC
    output.write(tempBuffer.data(), tempBuffer.size());
}

void VNAssembler::parseDotEntry(const std::string& line)
{
    std::string command = line.substr(0, line.find_first_of(" \t"));
    if(command.compare(".stack") == 0){
        stackSize = std::stoi(line.substr(line.find_first_of(" \t")));
    } else if(command.compare(".entry") == 0) {
        entryLabel = line.substr(command.size() + 1);
    }else if(command.compare(".ascii") == 0) {
        std::string data = line.substr(line.find_first_of("\"") + 1, line.find_last_of("\"") - line.find_first_of("\"") - 1);
        char instruction = ASCII;
        writeToTemp(&instruction, 1);
        Word size = static_cast<Word>(data.size());
        writeToTemp(reinterpret_cast<char*>(&size), sizeof(Word));
        writeToTemp(&PaddingValue, 1);
        writeToTemp(data.c_str(), size);
        char zero = 0;
        writeToTemp(&zero, 1);
        writeToTemp(&zero, 1);
    } else if(command.compare(".number") == 0){
        Word val = std::stoi(line.substr(line.find_first_of(" \t")));
        char instruction = NUMBER;
        writeToTemp(&instruction, 1);
        writeToTemp(&val, sizeof(Word));
    } else {
        assert(false);
    }
}

void VNAssembler::parseLabel(const std::string& line)
{
    std::string label = line.substr(0, line.size() - 1);
    assert(labelsToAddress.find(label) == labelsToAddress.end());
    labelsToAddress.insert({ label, executableSize });
}

void VNAssembler::parseInstruction(const std::string& line)
{
    std::string command = line.substr(0, line.find_first_of(" \t"));
    std::string args = line.substr(command.size());
    if(command.compare("mov") == 0) {
        parseMov(args);
    } else if(command.compare("push") == 0) {
        parsePush(args);
    } else if(command.compare("pop") == 0) {
        parsePop(args);
    } else if(command.compare("add") == 0) {
        parseAdd(args);
    } else if(command.compare("sub") == 0) {
        parseSub(args);
    } else if(command.compare("inc") == 0) {
        parseInc(args);
    } else if(command.compare("dec") == 0) {
        parseDec(args);
    } else if(command.compare("cmp") == 0) {
        parseCmp(args);
    } else if(command.compare("jeq") == 0) {
        parseJeq(args);
    } else if(command.compare("bl") == 0) {
        parseCall(args);
    } else if(command.compare("printf") == 0) {
        parsePrintf(args);
    } else if(command.compare("printn") == 0) {
        parsePrintn(args);
    } else if(command.compare("scann") == 0) {
        parseScann(args);
    } else {
        assert(false);
    }

}

void VNAssembler::addLabelToFill(std::string label, int index)
{
    auto found = labelsToFill.find(label);
    if(found == labelsToFill.end()) {
        labelsToFill.insert({label, std::vector<int>()});
        found = labelsToFill.find(label);
    }
    found->second.push_back(index);

}

void VNAssembler::parseMov(const std::string& line)
{
    std::string firstArg = StripFromSpaces(line.substr(line.find_first_of(" \t") + 1,
                                       line.find_first_of(",") - line.find_first_of(" \t") - 1));
    std::string secondArg = StripFromSpaces(line.substr(line.find_first_of(",") + 1));
    TRegisters firstRegister = ParseRegister(firstArg);
    if(secondArg[0] == '#') {
        assert(IsNumber(secondArg.substr(1)));
        Word number = std::stoi(secondArg.substr(1));
        char instruction = MOV_CONST;
        writeToTemp(&instruction, 1);
        writeToTemp(&firstRegister, 1);
        writeToTemp(&number, sizeof(Word));
    } else if(IsRegister(secondArg)) {
        char instruction = MOV;
        TRegisters secondRegister = ParseRegister(secondArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstRegister, 1);
        writeToTemp(&secondRegister, 1);
        writeToTemp(&PaddingValue, 1);
    } else {
        std::string label = StripFromSpaces(secondArg);
        Word tempVal = -1;
        char instruction = MOV_CONST;
        writeToTemp(&instruction, 1);
        writeToTemp(&firstRegister, 1);
        writeToTemp(&tempVal, sizeof(Word));
        int addressToFill = executableSize - sizeof(Word);
        addLabelToFill(label, addressToFill);
    }
}

void VNAssembler::parsePush(const std::string& line)
{
    if(line.find_first_of("{") == std::string::npos) {
        assert(line.find_first_of("#") != std::string::npos);
        std::string number = StripFromSpaces(line.substr(line.find_first_of("#") + 1));
        Word value = std::stoi(number);
        char instruction = PUSH_CONST;
        writeToTemp(&instruction, 1);
        writeToTemp(&value, sizeof(Word));
        writeToTemp(&PaddingValue, 1);
    }
    std::string firstArg = StripFromSpaces(line.substr(line.find_first_of("{") + 1,
                                       line.find_first_of(",-}") - line.find_first_of("{") - 1));
    char instruction = PUSH;
    TSeqVariant variant = NONE;
    if(line.find_first_of(",-") != std::string::npos) {
        std::string secondArg = StripFromSpaces(line.substr(line.find_first_of(",-") + 1, line.find_first_of("}") - line.find_first_of(",-") - 1));
        if(line.find_first_of(",") != std::string::npos) {
            variant = SEPARATE;
        } else {
            assert(line.find_first_of("-") != std::string::npos);
            variant = SEQUENCE;
        }
        TRegisters firstReg = ParseRegister(firstArg);
        TRegisters secondReg = ParseRegister(secondArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&variant, 1);
        writeToTemp(&secondReg, 1);
    } else {
        TRegisters firstReg = ParseRegister(firstArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&variant, 1);
        writeToTemp(&PaddingValue, 1);
    }
}

void VNAssembler::parsePop(const std::string& line)
{
    std::string firstArg = StripFromSpaces(line.substr(line.find_first_of("{") + 1,
                                       line.find_first_of(",-}") - line.find_first_of("{") - 1));
    char instruction = POP;
    TSeqVariant variant = NONE;
    if(line.find_first_of(",-") != std::string::npos) {
        std::string secondArg = StripFromSpaces(line.substr(line.find_first_of(",-") + 1, line.find("}") - line.find_first_of(",-") - 1));
        if(line.find_first_of(",") != std::string::npos) {
            variant = SEPARATE;
        } else {
            assert(line.find_first_of("-") != std::string::npos);
            variant = SEQUENCE;
        }
        TRegisters firstReg = ParseRegister(firstArg);
        TRegisters secondReg = ParseRegister(secondArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&variant, 1);
        writeToTemp(&secondReg, 1);
    } else {
        TRegisters firstReg = ParseRegister(firstArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&variant, 1);
        writeToTemp(&PaddingValue, 1);
    }
}

void VNAssembler::parseAdd(const std::string& line)
{
    std::string firstArg = StripFromSpaces(line.substr(0, line.find_first_of(",")));
    TRegisters firstReg = ParseRegister(firstArg);
    std::string secondArg = StripFromSpaces(line.substr(line.find_first_of(",")+1));
    if(secondArg[0] == '#') {
        char instruction = ADD_CONST;
        Word number = std::stoi(secondArg.substr(1));
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&number, sizeof(Word));
    } else {
        char instruction = ADD;
        TRegisters secondReg = ParseRegister(secondArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&secondReg, 1);
        writeToTemp(&PaddingValue, 1);
    }
}

void VNAssembler::parseSub(const std::string& line)
{
    std::string firstArg = StripFromSpaces(line.substr(0, line.find_first_of(",")));
    TRegisters firstReg = ParseRegister(firstArg);
    std::string secondArg = StripFromSpaces(line.substr(line.find_first_of(",")+1));
    if(secondArg[0] == '#') {
        char instruction = SUB_CONST;
        Word number = std::stoi(secondArg.substr(1));
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&number, sizeof(Word));
    } else {
        char instruction = SUB;
        TRegisters secondReg = ParseRegister(secondArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&secondReg, 1);
        writeToTemp(&PaddingValue, 1);
    }
}

void VNAssembler::parseInc(const std::string& line)
{
    std::string regArg = StripFromSpaces(line);
    TRegisters reg = ParseRegister(regArg);
    char instruction = INC;
    writeToTemp(&instruction, 1);
    writeToTemp(&reg, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
}

void VNAssembler::parseDec(const std::string& line)
{
    std::string regArg = StripFromSpaces(line);
    TRegisters reg = ParseRegister(regArg);
    char instruction = DEC;
    writeToTemp(&instruction, 1);
    writeToTemp(&reg, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
}

void VNAssembler::parseCmp(const std::string& line)
{
    std::string firstArg = StripFromSpaces(line.substr(0, line.find_first_of(",")));
    TRegisters firstReg = ParseRegister(firstArg);
    std::string secondArg = StripFromSpaces(line.substr(line.find_first_of(",")+1));
    if(secondArg[0] == '#') {
        char instruction = CMP_CONST;
        Word number = std::stoi(secondArg.substr(1));
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&number, sizeof(Word));
    } else {
        char instruction = CMP;
        TRegisters secondReg = ParseRegister(secondArg);
        writeToTemp(&instruction, 1);
        writeToTemp(&firstReg, 1);
        writeToTemp(&secondReg, 1);
        writeToTemp(&PaddingValue, 1);
    }
}

void VNAssembler::parseJeq(const std::string& line)
{
    std::string label = StripFromSpaces(line);
    char instruction = JEQ;
    Word tempVal = -1;
    writeToTemp(&instruction, 1);
    writeToTemp(&tempVal, sizeof(Word));
    writeToTemp(&PaddingValue, 1);
    int index = executableSize - sizeof(Word) - 1;
    addLabelToFill(label, index);
}

void VNAssembler::parseCall(const std::string& line)
{
    std::string label = StripFromSpaces(line);
    char instruction = CALL;
    Word tempVal = -1;
    writeToTemp(&instruction, 1);
    writeToTemp(&tempVal, sizeof(Word));
    writeToTemp(&PaddingValue, 1);
    int index = executableSize - sizeof(Word) - 1;
    addLabelToFill(label, index);
}

void VNAssembler::parsePrintf(const std::string& line)
{
    char instruction = PRINTF;
    writeToTemp(&instruction, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
}

void VNAssembler::parsePrintn(const std::string& line)
{
    char instruction = PRINTN;
    writeToTemp(&instruction, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
}

void VNAssembler::parseScann(const std::string& line)
{
    char instruction = SCANN;
    writeToTemp(&instruction, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
    writeToTemp(&PaddingValue, 1);
}
