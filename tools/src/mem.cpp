//
// Created by jiangshanfeng on 2023/6/25.
//

#include <timer.h>
#include <tool.h>

#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono;

char* cache(int64_t size) {
    cout << "cache size " << size << endl;
    return new char[size];
}

const int64_t kGb = 8 * 1024 * 1024 * 1024L;
char *destination;
const int kBatch = 4;
const int64_t kBatchSize = 1024 * 128L;
char source[kBatchSize];

void CopyBatch(char *src, int64_t start, int64_t end) {
    // TimerClock tc;
    for (auto i = start; i <= end; i += kBatchSize) {
        int64_t size = (i + kBatchSize <= end) ? kBatchSize : end - i;
        if (size != kBatchSize) {
            // cout <<"size " << size << ", i " << i << endl;
        }
        memcpy(destination + i, src, size);
    }
    // cout << "CopyBatch cost " << tc.getTimerMicroSec() << " us" << endl;
}

void single() {
    auto batch = kGb / kBatch;
    for (int64_t i = 0; i != kBatch; i++) {
        CopyBatch(source, i * batch, i + batch);
    }
}

void multiple() {
    auto latch = make_shared<CountDownLatch>(4);
    auto batch = kGb / 4;
    for (int64_t i = 0; i != 4; i++) {
        char *ptr = source;
        int64_t start = i * batch;
        int64_t end = start + batch;
        // cout << "thread " << i << ", start " << start << endl;
        thread([ptr, start, end, i, latch]() {
            CopyBatch(ptr, start, end);
            latch->count_down();
        }).detach();
    }
    latch->await();
}

int test() {
    TimerClock tm;
    destination = cache(kGb);
    for (int i = 0; i != kBatchSize; i++) {
        source[i] = (char)('a' + i);
    }
    cout << "malloc cache cost " << tm.getTimerMicroSec() << " us" << endl;
    tm.update();
    single();
    cout << "single copy cost " << tm.getTimerMicroSec() << " us" << endl;
    tm.update();
    multiple();
    cout << "multiple copy cost " << tm.getTimerMicroSec() << " us" << endl;
    delete destination;
    return 0;
}