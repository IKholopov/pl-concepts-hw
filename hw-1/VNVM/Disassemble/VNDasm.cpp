#include <VNDisassembler.h>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    std::string inputPath;
    std::string outputPath;
    switch (argc) {
    case 2:
        inputPath = argv[1];
        outputPath = inputPath + ".vn";
        break;
    case 3:
        inputPath = argv[1];
        outputPath = argv[2];
        break;
    default:
        std::cout << "Not enough (or too many) arguments" << std::endl;
        break;
    }
    VNDisassembler disassembly;
    std::ifstream input(inputPath, std::ifstream::in);
    std::ofstream output(outputPath, std::ofstream::out);
    disassembly.Parse(input, output);
    return 0;
}
