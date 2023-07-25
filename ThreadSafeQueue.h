#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <mutex>
#include <queue>
#include <thread>

/**
 * ThreadSafeQueue is a warpper around the std::queue class. This wrapper adds a mutex to make it thread safe
 * 
 * This class is functionally the same as std::queue, except the pop() function both removes the first element of the queue, and returns that element
 * 
 * @tparam C the type of data the queue stores
 */
template <class C>
class ThreadSafeQueue {
    public:
        ThreadSafeQueue();
        ThreadSafeQueue(int sLockMilliseconds);
        C safePop();
        ThreadSafeQueue<C>& operator=(const ThreadSafeQueue<C>& copy);
        bool safePop(C* output);
        void push(C data);
        bool empty();
        int size();
    private:
        std::queue<C> queue;
        std::mutex mu;
        std::chrono::milliseconds sleepLockMilliseconds;
};

/**
 * Thread
 * 
 * @tparam C the type of data the queue stores 
 */
template<class C>
inline ThreadSafeQueue<C>::ThreadSafeQueue() {
    sleepLockMilliseconds = std::chrono::milliseconds(5);
}

template<class C>
inline ThreadSafeQueue<C>::ThreadSafeQueue(int sLockMilliseconds) {
    if(sLockMilliseconds > 0)
        sleepLockMilliseconds = std::chrono::milliseconds(sLockMilliseconds);
    else
        sleepLockMilliseconds = std::chrono::milliseconds(1);
}


template <class C>
inline C ThreadSafeQueue<C>::safePop() {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    C temp;
    if(!queue.empty()) {
        temp = queue.front();
        queue.pop();
    }
    return temp;
}

template <class C>
inline bool ThreadSafeQueue<C>::safePop(C* output) {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    if(!queue.empty()) {
        *output = queue.front();
        queue.pop();
        return true;
    }
    return false;
}

template <class C>
inline void ThreadSafeQueue<C>::push(C data) {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    queue.push(data);
}

template <class C>
inline bool ThreadSafeQueue<C>::empty() {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    bool temp = queue.empty();
    return temp;
}

template <class C>
inline int ThreadSafeQueue<C>::size() {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    int temp = queue.size();
    return temp;
}


template<class C>
inline ThreadSafeQueue<C>& ThreadSafeQueue<C>::operator=(const ThreadSafeQueue<C>& copy) {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    std::queue<C> empty;
    std::swap(queue, empty);
    std::queue<C> temp = copy.queue;

    while(temp.front) {
        queue.insert(temp.front());
        temp.pop();
    }
    return *this;
}

#endif