#include <FullReadVNVMProvider.h>
#include <assert.h>

static const int RegistersOffset = 2 * sizeof(Word);
static const int RegistersSize = sizeof(Word);

FullReadNVVMProvider::FullReadNVVMProvider(std::istream& inputStream) :
    stackSize(0),
    registersCount(RegistersSize),
    executableSize(0)
{
    bool readStatus = static_cast<bool>(inputStream.read(reinterpret_cast<char*>(&stackSize), sizeof(Word)));
    assert(readStatus);
    assert(stackSize > 0);
    readStatus = static_cast<bool>(inputStream.read(reinterpret_cast<char*>(&executableSize), sizeof(Word)));
    assert(readStatus);
    assert(executableSize > 0);
    inputStream.seekg(0, std::ios::end);
    int fullSize = inputStream.tellg();
    assert(executableSize > RegistersOffset + registersCount * RegistersSize);
    assert(fullSize <= executableSize + stackSize);
    inputStream.seekg(0, std::ios::beg);
    values.resize(executableSize + stackSize);
    readStatus = static_cast<bool>(inputStream.read(&values[0], fullSize));
    assert(readStatus);
}

Word* FullReadNVVMProvider::GetRegistryAt(std::byte i)
{
    unsigned char registerIndex = static_cast<unsigned char>(i);
    return reinterpret_cast<Word*>(values.data() + RegistersOffset + RegistersSize * registerIndex);
}

char* FullReadNVVMProvider::GetStackPtr()
{
    return reinterpret_cast<char*>(&values[executableSize]);
}
