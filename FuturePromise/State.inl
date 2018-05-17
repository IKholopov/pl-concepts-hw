template<typename T>
void SharedState<T>::Wait()
{
    std::unique_lock<std::mutex> lk(critical);
    cv.wait(lk, [this]() {
        if(value.get() == nullptr && exception == nullptr && terminated) {
            throw DanglingFutureException{};
        }
        return value.get() != nullptr || exception != nullptr;
    });
}

template<typename T>
T* SharedState<T>::GetValue()
{
    std::lock_guard<std::mutex> guard(critical);
    if(exception != nullptr) {
        std::rethrow_exception(exception);
    }
    return value.get();
}

template<typename T>
T* SharedState<T>::GetOrSetValue(const T& v)
{
    std::lock_guard<std::mutex> guard(critical);
    if(exception != nullptr) {
        std::rethrow_exception(exception);
    }
    if(value != nullptr) {
        return value.get();
    }
    value = std::move(std::make_unique<T>(v));
    cv.notify_all();
    return value.get();
}

template<typename T>
void SharedState<T>::TryThrow() const
{
    std::lock_guard<std::mutex> guard(critical);
    if(exception != nullptr) {
        std::rethrow_exception(exception);
    }
}

template<typename T>
void SharedState<T>::SetValue(const T& v)
{
    std::lock_guard<std::mutex> lock(critical);
    if(value != nullptr || exception != nullptr) {
        throw MultipleSetPromiseException {};
    }
    value = std::move(std::make_unique<T>(v));
    cv.notify_all();
}

template<typename T>
void SharedState<T>::SetException()
{
    std::lock_guard<std::mutex> lock(critical);
    if(value != nullptr || exception != nullptr) {
        throw MultipleSetPromiseException {};
    }
    exception = std::current_exception();
    cv.notify_all();
}

template<typename T>
void SharedState<T>::Terminate()
{
    std::lock_guard<std::mutex> lock(critical);
    terminated = true;
    cv.notify_all();
}
