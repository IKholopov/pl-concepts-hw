#include <iostream>

#include <Primitives.h>

class ItsAllOverNowException : public Exception {
public:
    virtual std::string what() override { return "It's all over now"; }
};

class CompletelyDifferentException : public Exception {
public:
    virtual std::string what() override { return "Completely different exception"; }
};

int func(Int32 n) {
    if(n() < 0) {
        THROW(new ItsAllOverNowException());
    }
    if(n() == 0) {
        return 0;
    }
    Int32 a(n);
    Int32 b(6);
    if(n() > 5) {
        TRY {
            return func(n() - 2) + 1;
        } CATCH(ItsAllOverNowException) {
            std::cout << Exception::GetActiveException<ItsAllOverNowException>()->what() << std::endl;
            return 9000;
        }
    }
    return func(n() - 2) + 1;
}

void test1(int val) {
    std::cout << func(val) << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////


int TwoTypes(Int32 n) {
    if(n() < 0) {
        THROW(new CompletelyDifferentException());
    }
    if(n() == 0) {
        return 0;
    }
    Int32 a(n);
    Int32 b(6);
    if( n() > 10 ) {
        TRY {
            if(n() > 5) {
                TRY {
                    return TwoTypes(n() - 2) + 1;
                } CATCH(ItsAllOverNowException) {
                    std::cout << Exception::GetActiveException<ItsAllOverNowException>()->what() << std::endl;
                    return 9000;
                }
            }
        } CATCH(CompletelyDifferentException) {
            std::cout << Exception::GetActiveException<CompletelyDifferentException>()->what() << std::endl;
            return 9000;
        }
    }
    return TwoTypes(n() - 2) + 1;
}

void test2() {
    std::cout << TwoTypes(21) << std::endl;
}

int main()
{
    std::cout << "___test1(21)___" << std::endl;
    test1(21);
    std::cout << "_______________" << std::endl;
    std::cout << "___test1(22)___" << std::endl;
    test1(22);
    std::cout << "_______________" << std::endl;
    std::cout << "___test2()_____" << std::endl;
    test2();
    std::cout << "_______________" << std::endl;
    return 0;
}
