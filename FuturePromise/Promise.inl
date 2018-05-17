template<typename T>
Promise<T>::Promise(): state(new SharedState<T>)
{
}

template<typename T>
Promise<T>::Promise(Promise<T>&& other): state(other.state)
{
}
