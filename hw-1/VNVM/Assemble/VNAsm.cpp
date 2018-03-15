#include <iostream>
#include <fstream>

#include <VNAssembler.h>
#include <assert.h>

int main(int argc, char** argv) {
    std::string inputPath;
    std::string outputPath;
    switch (argc) {
    case 2:
        inputPath = argv[1];
        outputPath = inputPath + ".bin";
        break;
    case 3:
        inputPath = argv[1];
        outputPath = argv[2];
        break;
    default:
        std::cout << "Not enough (or too many) arguments" << std::endl;
        break;
    }
    VNAssembler assembly;
    std::ifstream input(inputPath, std::ifstream::in);
    std::ofstream output(outputPath, std::ofstream::out);
    assembly.Parse(input, output);
    return 0;
}
