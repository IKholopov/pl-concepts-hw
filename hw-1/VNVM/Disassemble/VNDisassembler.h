#pragma once

#include <CommonDefines.h>
#include <fstream>
#include <assert.h>

class VNDisassembler {
public:
    VNDisassembler();

    void Parse(std::istream& input, std::ostream& output);

    template<typename T>
    void ReadFromInput(std::istream& input, T* to, int size) {
        assert(!input.eof());
        input.read(reinterpret_cast<char*>(to), size);
        readSize += size;
    }

private:
    Word stackSize;
    Word readSize;
    Word executableSize;

    void parseAcii(std::istream& input, std::ostream& output);
    void parseNumber(std::istream& input, std::ostream& output);
    void parseMov(std::istream& input, std::ostream& output);
    void parseMovConst(std::istream& input, std::ostream& output);
    void parsePush(std::istream& input, std::ostream& output);
    void parsePushConst(std::istream& input, std::ostream& output);
    void parsePop(std::istream& input, std::ostream& output);
    void parseAdd(std::istream& input, std::ostream& output);
    void parseAddConst(std::istream& input, std::ostream& output);
    void parseSub(std::istream& input, std::ostream& output);
    void parseSubConst(std::istream& input, std::ostream& output);
    void parseInc(std::istream& input, std::ostream& output);
    void parseDec(std::istream& input, std::ostream& output);
    void parseCmp(std::istream& input, std::ostream& output);
    void parseCmpConst(std::istream& input, std::ostream& output);
    void parseJeq(std::istream& input, std::ostream& output);
    void parseCall(std::istream& input, std::ostream& output);
    void parsePrintf(std::istream& input, std::ostream& output);
    void parsePrintn(std::istream& input, std::ostream& output);
    void parseScann(std::istream& input, std::ostream& output);
};
