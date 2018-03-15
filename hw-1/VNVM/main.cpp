#include <iostream>
#include <fstream>

#include <VAS16.h>
#include <FullReadVNVMProvider.h>
#include <Instruction.h>
#include <VNProcess.h>

std::unique_ptr<VNProcess> ActiveProcess;

bool isProcessLoaded() {
    if(ActiveProcess.get() == nullptr) {
        std::cout << "No active process loaded" << std::endl;
        return false;
    }
    return true;
}

void loadProcess(std::string path) {
    std::ifstream reader(path, std::ios::binary | std::ios::in);
    auto exeRegProvider = std::make_unique<FullReadNVVMProvider>(reader);
    auto vas = std::make_unique<VAS16>(exeRegProvider.get());
    vas->SetStackPtr(exeRegProvider->GetStackPtr());
    std::unique_ptr<IVirtualAddressSpace<Word>> addressSpace = std::move(vas);
    std::unique_ptr<IRegistryProvider> regProvider = std::move(exeRegProvider);
    ActiveProcess.reset(new VNProcess(addressSpace, regProvider));
}

void run() {
    if(isProcessLoaded()) {
        while (ActiveProcess->Iteration()) {
        }
        std::cout << "Process terminated" << std::endl;
    }
}

void step() {
    if(isProcessLoaded()) {
        ActiveProcess->Iteration();
    }
}

void registers() {
    if(isProcessLoaded()) {
        ActiveProcess->DumpRegisters();
    }
}

void stack(int n) {
    if(isProcessLoaded()) {
        ActiveProcess->DumpStack(n);
    }
}

void dump(std::string dump) {
    if(isProcessLoaded()) {
        ActiveProcess->DumpImage(dump);
    }
}

int main(int argc, char** argv)
{
    if(argc > 1) {
        std::string path = argv[1];
        loadProcess(path);
    }
    std::string command;
    std::cin >> command;
    while (command.compare("exit") != 0) {
        if(command.compare("load") == 0) {
            std::string path;
            std::cin >> path;
            loadProcess(path);
        } else if(command.compare("run") == 0) {
            run();
        } else if(command.compare("step") == 0) {
            step();
        } else if(command.compare("regs") == 0) {
            registers();
        } else if(command.compare("stack") == 0) {
            int topN;
            std::cin >> topN;
            stack(topN);
        } else if(command.compare("dump") == 0) {
            std::string path;
            std::cin >> path;
            dump(path);
        } else {
            std::cout << "Not a command. Valid commands:" << std::endl <<
                         "\texit" << std::endl <<
                         "\tload <file>" << std::endl <<
                         "\trun" << std::endl <<
                         "\tstep" << std::endl <<
                         "\tregs" << std::endl <<
                         "\tstack <N>" << std::endl <<
                         "\tdump <file>" << std::endl;
        }
        std::cin >> command;
    }
    return 0;
}
