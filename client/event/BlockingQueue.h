#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <condition_variable>
#include <mutex>
#include <queue>

template<typename T>
class BlockingQueue {
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;

public:
    void enqueue(T item) {
        std::lock_guard lock(_mutex);
        _queue.push(std::move(item));
        _cond.notify_one();
    }

    T take() {
        std::unique_lock lock(_mutex);
        _cond.wait(lock, [this] { return !_queue.empty(); });
        const T item = std::move(_queue.front());
        _queue.pop();
        return item;
    }

    bool tryPop(T &item) {
        std::unique_lock lock(_mutex);
        if (_queue.empty()) {
            return false;
        }
        item = std::move(_queue.front());
        _queue.pop();
        return true;
    }
};


#endif //BLOCKINGQUEUE_H
