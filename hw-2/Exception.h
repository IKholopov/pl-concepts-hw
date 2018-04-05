#pragma once

#include <setjmp.h>
#include <assert.h>
#include <string>
#include <memory>


class Stackable {
public:
    Stackable();
    virtual ~Stackable();

private:
    Stackable* previous;
    bool onStack;
};

class TryEntry {
public:
    TryEntry();
    ~TryEntry();

    jmp_buf& operator()() { return env; }

    static jmp_buf& GetActive();
    static void Unwind();

private:
    jmp_buf env;
    TryEntry* prev;
    Stackable* lastStackable;

    void doUnwind();
};

class Exception {
public:
    Exception();
    virtual ~Exception();

    virtual std::string what() = 0;

    static Exception* GetActive();
    static void FreeActive();
    template<typename T>
    static std::unique_ptr<T> GetActiveException() {
        auto val = dynamic_cast<T*>(GetActive());
        assert(val != nullptr);
        FreeActive();
        return std::unique_ptr<T>(val);
    }

    static bool IsCatched();
    static void Catch();
    //static void UnCatch();
};

#define TRY { \
    TryEntry ___tryEntry; \
    if(!setjmp(___tryEntry()))

#define CATCH(ExceptionType) else { \
    if(dynamic_cast<ExceptionType*>(Exception::GetActive())) { Exception::Catch(); }  \
} } if(!Exception::IsCatched()) { if(Exception::GetActive() != 0) { TryEntry::Unwind(); longjmp(TryEntry::GetActive(), 0); } } else

#define THROW(ExceptionPtr) ExceptionPtr; TryEntry::Unwind(); longjmp(TryEntry::GetActive(), 0)
