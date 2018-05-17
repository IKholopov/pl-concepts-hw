template<typename T>
const T& Future<T>::Get()
{
    state->Wait();
    return *state->GetValue();
}

template<typename T>
bool Future<T>::TryGet(T& value)
{
    T* current = state->GetValue();
    if(current == nullptr) {
        return false;
    }
    value = *current;
    return true;
}

template<typename T, typename R>
R* WrappedState<T, R>::GetValue()
{
    return this->GetOrSetValue(processor(future.Get()));
}

template<typename T>
template<typename R>
Future<R> Future<T>::Then(std::function<R (const T&)> process)
{
    return Future<R>(std::make_shared<WrappedState<T,R>>(*this, process));
}
