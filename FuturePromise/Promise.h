#pragma once

#include <memory>
#include <condition_variable>
#include <mutex>
#include <functional>

#define interface struct

struct MultipleSetPromiseException : public std::exception
{
    const char* what() const throw ()
    {
        return "Trying to set promise value multiple times";
    }
};

struct DanglingFutureException : public std::exception
{
    const char* what() const throw ()
    {
        return "The future is dangling";
    }
};

template<typename T>
class SharedState {
public:
    SharedState(): exception(nullptr), terminated(false) {}

    virtual void Wait();
    virtual T* GetValue();
    T* GetOrSetValue(const T&v);
    void SetValue(const T& v);
    void SetException();
    void TryThrow() const;
    void Terminate();

private:
    std::unique_ptr<T> value;
    std::exception_ptr exception;
    std::mutex critical;
    std::condition_variable cv;
    bool terminated;
};

template <typename T>
class Future {
public:
    explicit Future(const std::shared_ptr<SharedState<T>>& state): state(state) {}
    Future(const Future& other): state(other.state) {}
    Future(Future&& other): state(other.state) {}

    const T& Get();
    bool TryGet(T& value);
    template<typename R>
    Future<R> Then(std::function<R(const T&)> process);

private:
    std::shared_ptr<SharedState<T>> state;
};

template<typename T, typename R>
class WrappedState: public SharedState<R> {
public:
    WrappedState(Future<T> future, std::function<R(const T&)> worker):
        future(future),
        processor(worker)
    {}

    virtual void Wait() override { return; }
    virtual R* GetValue() override;

private:
    Future<T> future;
    std::function<R(const T&)> processor;
};

template <typename T>
class Promise {
public:
    Promise();
    Promise(Promise&& other);
    Promise(const Promise& other) = delete;
    ~Promise() { state->Terminate(); }

    void Set(const T& value) { state->SetValue(value); }
    void SetException(){ state->SetException(); }

    Future<T> Result() { return Future<T>(state); }

private:
    std::shared_ptr<SharedState<T>> state;
};

#include <State.inl>
#include <Future.inl>
#include <Promise.inl>
