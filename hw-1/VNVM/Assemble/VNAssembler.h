#pragma once

#include <CommonDefines.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

class VNAssembler {
public:
    VNAssembler();
    void Parse(std::ifstream& input, std::ofstream& output);

private:
    std::vector<char> tempBuffer;
    std::unordered_map<std::string, std::vector<int>> labelsToFill;
    std::unordered_map<std::string, Word> labelsToAddress;
    std::string entryLabel;
    Word stackSize;
    Word entryAddress;
    Word executableSize;

    void parseDotEntry(const std::string& line);
    void parseLabel(const std::string& line);
    void parseInstruction(const std::string& line);

    void addLabelToFill(std::string label, int index);
    void parseMov(const std::string& line);
    void parsePush(const std::string& line);
    void parsePop(const std::string& line);
    void parseAdd(const std::string& line);
    void parseSub(const std::string& line);
    void parseInc(const std::string& line);
    void parseDec(const std::string& line);
    void parseCmp(const std::string& line);
    void parseJeq(const std::string& line);
    void parseCall(const std::string& line);
    void parsePrintf(const std::string& line);
    void parsePrintn(const std::string& line);
    void parseScann(const std::string& line);

    template<typename T>
    void writeToTemp(T* valPtr, Word size) {
        for(int i = 0; i < size; ++i) {
            tempBuffer.push_back(*(reinterpret_cast<const char*>(valPtr) + i));
        }
        executableSize += size;
    }
};
