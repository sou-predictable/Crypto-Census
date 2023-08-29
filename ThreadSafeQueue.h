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

        /**
         * Default constructor.
         * 
         * Initializes sleepLockMilliseconds, the number of miliseconds a thread should sleep after encountering a conflict
         * 
         * @tparam C the type of data the queue stores. 
         */
        ThreadSafeQueue();
        
        /**
         * Constructor.
         * 
         * Initializes sleepLockMilliseconds, the number of miliseconds a thread should sleep after encountering a conflict.
         * If the parameter passed is equal or less 0, sleepLockMiliseconds defaults to 1.
         * 
         * @tparam C the type of data the queue stores. 
         * @param sLockMiliseconds the number of miliseconds a thread should sleep after encountering a conflict 
         */
        ThreadSafeQueue(int sLockMilliseconds);

        /**
         * Thread safe wrapper around queue.pop().
         * 
         * Unlike the STD counterpart, pop() both removes the first element and returns it.
         * 
         * @tparam C the type of data the queue stores.
         * @return C the item at the front of the queue.
         */
        C safePop();

        /**
         * Thread safe wrapper around queue.pop().
         * 
         * @tparam C the type of data the queue stores.
         * @param[out] output the object popped from the queue.
         * @return true if successfully popped, false otherwise.
         */
        bool safePop(C* output);
        
        /**
         * Thread safe wrapper around queue.push().
         * 
         * @tparam C the type of data the queue stores.
         * @param data the object to be pushed to the queue.
         */
        void push(C data);

        /**
         * Thread safe wrapper around queue.empty().
         * 
         * @tparam C the type of data the queue stores.
         * @return true if the queue is empty, false otherwise.
         */
        bool empty();
        
        /**
         * Thread safe wrapper around queue.size().
         * 
         * @tparam C the type of data the queue stores.
         * @return the size of the queue.
         */
        int size();

        /**
         * Thread safe wrapper around queue's copy constructor.
         * 
         * @tparam C the type of data the queue stores.
         * @return a reference to the local variable.
         */
        ThreadSafeQueue<C>& operator=(const ThreadSafeQueue<C>& copy);
        
    private:
        std::queue<C> queue;
        std::mutex mu;
        std::chrono::milliseconds sleepLockMilliseconds;
};

template<class C>
inline ThreadSafeQueue<C>::ThreadSafeQueue() {
    sleepLockMilliseconds = std::chrono::milliseconds(1);
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