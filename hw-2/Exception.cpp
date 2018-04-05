#include <Exception.h>

#include <cstdlib>

static Stackable* activeStackPointer;
static TryEntry* activeTry;
static bool allocateOnHeap;

static Exception* activeException;
static bool exceptionCatched;

void UnwindTo(Stackable* value) {
    while(value != activeStackPointer) {
        assert(activeStackPointer != nullptr);
        activeStackPointer->~Stackable();
    }
}

Stackable::Stackable() :
    previous(activeStackPointer),
    onStack(!allocateOnHeap)
{
    allocateOnHeap = false;
    if(onStack) {
        activeStackPointer = this;
    }
}

Stackable::~Stackable()
{
    if(onStack) {
        activeStackPointer = previous;
    }
}

void* operator new(std::size_t n)
{
    allocateOnHeap = true;
    return std::malloc(n);
}

TryEntry::TryEntry() :
    prev(activeTry),
    lastStackable(activeStackPointer)
{
    activeTry = this;
}

TryEntry::~TryEntry()
{
    activeTry = prev;
}

jmp_buf& TryEntry::GetActive()
{
    return activeTry->env;
}

void TryEntry::Unwind()
{
    if(activeTry == nullptr) {
        std::abort();
    }
    activeTry->doUnwind();
}

void TryEntry::doUnwind()
{
    UnwindTo(lastStackable);
}

Exception::Exception()
{
    if(GetActive() != nullptr) {
        std::abort();
    }
    activeException = this;
}

Exception::~Exception()
{
    activeException = nullptr;
}

Exception* Exception::GetActive()
{
    return activeException;
}

void Exception::FreeActive()
{
    activeException = nullptr;
}

bool Exception::IsCatched()
{
    bool status = exceptionCatched;
    exceptionCatched = false;
    return status;
}

void Exception::Catch()
{
    exceptionCatched = true;
}
