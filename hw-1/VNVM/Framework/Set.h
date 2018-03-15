#pragma once

#include <cstddef>
#include <assert.h>

template<std::byte Size>
class Set {
public:
    Set();
    bool Has(std::byte value) const;

    void operator |= (const Set& other);
    void operator |= (std::byte value);
    void operator |= (int value) { assert(value < 256 && value > 0); this->operator |=(std::byte(value)); }

    void operator &= (const Set& other);
    void operator &= (std::byte value);
    void operator &= (int value) { assert(value < 256 && value > 0); this->operator &=(std::byte(value)); }

private:
    std::byte data[static_cast<char>(Size) / (sizeof(std::byte) * 8)];
    const std::byte pages = std::byte(static_cast<char>(Size) / (sizeof(std::byte) * 8));
    const std::byte maxSize = Size;
    std::byte size;
};

template<std::byte Size>
Set<Size>::Set()
{
    for(int i = 0; i < static_cast<int>(pages); ++i) {
        data[i] = std::byte(0);
    }
}

template<std::byte Size>
bool Set<Size>::Has(std::byte valueB) const
{
    unsigned char value = static_cast<unsigned char>(valueB);
    unsigned char pageId = value / 8;
    std::byte selectedPage = std::byte(1 << (value % 8));
    return static_cast<bool>(data[pageId] & selectedPage);
}

template<std::byte Size>
void Set<Size>::operator |=(const Set& other)
{
    for( int i = 0; i < static_cast<int>(pages); ++i) {
        data[i] |= other.data[i];
    }
}

template<std::byte Size>
void Set<Size>::operator |=(std::byte valueB)
{
    unsigned char value = static_cast<unsigned char>(valueB);
    unsigned char pageId = value / 8;
    std::byte selectedPage = std::byte(1 << (value % 8));
    data[pageId] |= selectedPage;
}

template<std::byte Size>
void Set<Size>::operator &=(const Set& other)
{
    for( int i = 0; i < static_cast<int>(pages); ++i) {
        data[i] &= other.data[i];
    }
}

template<std::byte Size>
void Set<Size>::operator &=(std::byte valueB)
{
    unsigned char value = static_cast<unsigned char>(valueB);
    std::byte pageId = value / 8;
    std::byte selectedPage = 1 << (value % 8);
    data[pageId] &= selectedPage;
}
