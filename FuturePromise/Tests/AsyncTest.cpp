#include <Async.h>

#include <gtest/gtest.h>

TEST( ThreadPool, Tasks ) {
    ThreadPool pool(4);
    int size = 1;
    bool done[size];
    for(int i = 0; i < size; ++i) {
        done[i] = false;
    }

    for(int i = 0; i < size; ++i) {
        pool.AddTask([&done, i]() {
            done[i] = true;
        });
    }

    pool.Terminate();

    for(int i = 0; i < size; ++i) {
        ASSERT_TRUE(done[i]);
    }
}
