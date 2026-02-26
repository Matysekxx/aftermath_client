#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * @brief A thread-safe queue for passing data between threads.
 *
 * This template class provides a standard blocking queue implementation using
 * std::mutex and std::condition_variable. It is used for communication between
 * the network threads and the main game loop.
 *
 * @tparam T The type of elements stored in the queue.
 */
template<typename T>
class BlockingQueue {
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;

public:
    /**
     * @brief Adds an item to the queue.
     * @param item The item to add.
     */
    void enqueue(T item) {
        std::lock_guard lock(_mutex);
        _queue.push(std::move(item));
        _cond.notify_one();
    }

    /**
     * @brief Removes and returns an item from the queue, blocking if empty.
     * @return The item from the front of the queue.
     */
    T take() {
        std::unique_lock lock(_mutex);
        _cond.wait(lock, [this] { return !_queue.empty(); });
        const T item = std::move(_queue.front());
        _queue.pop();
        return item;
    }

    /**
     * @brief Tries to remove and return an item from the queue without blocking.
     * @param item Reference where the popped item will be stored.
     * @return True if an item was popped, false if the queue was empty.
     */
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
