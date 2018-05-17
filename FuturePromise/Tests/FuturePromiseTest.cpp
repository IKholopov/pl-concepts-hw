#include <gtest/gtest.h>

#include <Promise.h>
#include <thread>
#include <condition_variable>

TEST( SharedState, GetSet ) {
    SharedState<int> state;
    int value = 9000;

    std::thread thread1([&state, value]() {
        state.Wait();
        ASSERT_EQ(value, *state.GetValue());
    });

    sleep(2);
    state.SetValue(value);

    std::thread thread2([&state, value]() {
        state.Wait();
        ASSERT_EQ(value, *state.GetValue());
    });
    std::thread thread3([&state, value]() {
        state.Wait();
        ASSERT_EQ(value, *state.GetValue());
    });

    thread1.join();
    thread2.join();
    thread3.join();
}

TEST( SharedState, Exception ) {
    SharedState<int> state;
    bool catched = false;

    std::thread thread1([&state]() {
        sleep(1);
        try{
            throw std::logic_error("");
        } catch(...) {
            state.SetException();
        }
    });

    try {
        state.Wait();
        state.GetValue();
    } catch(std::logic_error& e) {
        catched = true;
    }

    thread1.join();
    ASSERT_TRUE(catched);
}


TEST( FuturePromise, GetSet ) {
    Promise<int> promise;
    int value = 66;
    auto future = promise.Result();
    std::thread thread1([&promise, value]() {
        Promise<int> p = std::move(promise);
        sleep(2);
        p.Set(value);
    });

    ASSERT_EQ(value, future.Get());
    thread1.join();
}

TEST( FuturePromise, GetMultipleSet ) {
    Promise<int> promise;
    int value = 66;
    auto future = promise.Result();

    std::vector<std::thread> threads;

    for(int i = 0; i < 10; ++i) {
        threads.push_back(std::thread([future, value]() {
            Future<int> f = future;
            ASSERT_EQ(value, f.Get());
        }));
    }

    std::thread thread1([&promise, value]() {
        Promise<int> p = std::move(promise);
        sleep(2);
        p.Set(value);
    });

    for(int i = 0; i < 10; ++i) {
        threads.push_back(std::thread([future, value]() {
            Future<int> f = future;
            ASSERT_EQ(value, f.Get());
        }));
    }

    thread1.join();
    for(auto& t: threads) {
        t.join();
    }
}

TEST( FuturePromise, GetException ) {
    Promise<int> promise;
    bool catched = false;

    auto future = promise.Result();
    std::thread thread1([&promise]() {
        try{
            sleep(2);
            throw std::logic_error("");
        } catch(std::exception& e) {
            promise.SetException();
        }
    });

    try {
        int r = future.Get();
    } catch(std::logic_error& e) {
        catched = true;
    }
    ASSERT_TRUE(catched);
    thread1.join();
}

TEST( FuturePromise, TryGet ) {
    Promise<int> promise;
    int value = 99;
    bool tried = false;
    bool set = false;
    std::mutex critical;
    std::condition_variable cv;

    auto future = promise.Result();
    std::thread thread1([&promise, &critical, &cv, &tried, value, &set]() {
        std::unique_lock<std::mutex> lk(critical);
        cv.wait(lk, [&tried](){ return tried; });
        promise.Set(value);
        set = true;
        std::cout << "Set" << std::endl;
        cv.notify_all();
    });

    int oldR = -1;
    int r = oldR;
    ASSERT_FALSE(future.TryGet(r));
    ASSERT_EQ(oldR, r);
    tried = true;
    std::cout << "Tried" << std::endl;
    cv.notify_all();

    {
        std::unique_lock<std::mutex> lk(critical);
        cv.wait(lk, [&set](){ return set; });
        ASSERT_TRUE(future.TryGet(r));
    }

    ASSERT_EQ(value, r);
    thread1.join();
}

TEST( FuturePromise, TryGetException ) {
    Promise<int> promise;
    int value = 99;
    bool tried = false;
    bool set = false;
    bool catched = false;
    std::mutex critical;
    std::condition_variable cv;

    auto future = promise.Result();
    std::thread thread1([&promise, &critical, &cv, &tried, value, &set]() {
        std::unique_lock<std::mutex> lk(critical);
        cv.wait(lk, [&tried](){ return tried; });
        try{
            throw std::logic_error("");
            promise.Set(value);
        } catch(...) {
            promise.SetException();
        }
        set = true;
        std::cout << "Set" << std::endl;
        cv.notify_all();
    });

    int oldR = -1;
    int r = oldR;
    ASSERT_FALSE(future.TryGet(r));
    ASSERT_EQ(oldR, r);
    tried = true;
    std::cout << "Tried" << std::endl;
    cv.notify_all();

    {
        std::unique_lock<std::mutex> lk(critical);
        cv.wait(lk, [&set](){ return set; });
        try{
            future.TryGet(r);
        } catch(std::logic_error& e) {
            catched = true;
        }

    }
    ASSERT_TRUE(catched);
    thread1.join();
}

TEST( FuturePromise, Lifetime ) {
    Promise<int> promise;
    int value = 66;
    bool catched = false;
    auto future = promise.Result();
    std::thread thread1([&promise, value]() {
        Promise<int> p = std::move(promise);
        sleep(2);
        //p.Set(value);
    });

    try {
        ASSERT_EQ(value, future.Get());
    } catch(DanglingFutureException& e) {
        catched = true;
    }

    ASSERT_TRUE(catched);
    thread1.join();
}

TEST( FuturePromise, GetThen ) {
    Promise<int> promise;
    int value = 66;
    auto future = promise.Result();
    std::thread thread1([&promise, value]() {
        Promise<int> p = std::move(promise);
        sleep(1);
        p.Set(value);
    });

    auto halfed = future.Then<double>([](int v){ return static_cast<double>(v) / 2; });
    auto copy = halfed;
    ASSERT_FLOAT_EQ(static_cast<double>(value) / 2, halfed.Get());

    std::thread thread2([halfed, value]() mutable {
        ASSERT_FLOAT_EQ(static_cast<double>(value) / 2, halfed.Get());
    });

    thread1.join();
    thread2.join();
}

TEST( FuturePromise, GetThenException ) {
    Promise<int> promise;
    int value = 66;
    bool catched1 = false;
    bool catched2 = false;
    auto future = promise.Result();
    std::thread thread1([&promise, value]() {
        Promise<int> p = std::move(promise);
        sleep(1);
        try {
            throw std::logic_error("");
        }catch(...) {
            p.SetException();
        }
    });

    auto halfed = future.Then<double>([](int v){ return static_cast<double>(v) / 2; });
    auto copy = halfed;
    try{
        ASSERT_FLOAT_EQ(static_cast<double>(value) / 2, halfed.Get());
    } catch(std::logic_error& e) {
        catched1 = true;
    }
    ASSERT_TRUE(catched1);
    std::thread thread2([halfed, value, &catched2]() mutable {
        try{
            ASSERT_FLOAT_EQ(static_cast<double>(value) / 2, halfed.Get());
        } catch(std::logic_error& e) {
            catched2 = true;
        }
        ASSERT_TRUE(catched2);
    });

    thread2.join();
    thread1.join();
}

int main(int argc, char** argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
