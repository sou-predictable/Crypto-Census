#ifndef THREADSAFESET_H
#define THREADSAFESET_H

#include <mutex>
#include <thread>
#include <unordered_set>

/**
 * ThreadSafeSet is a wrapper class for std::set. This wrapper adds a mutex to make it thread safe
 */
template <class C>
class ThreadSafeSet {
    public:
        ThreadSafeSet();
        ThreadSafeSet(int sLockMilliseconds);
        bool contains(C data);
        bool safeInsert(C data);
        ThreadSafeSet& operator=(const ThreadSafeSet& copy);
    private:
        std::mutex mu;
        std::unordered_set<C> set;
        std::chrono::milliseconds sleepLockMilliseconds;
};

template<class C>
inline ThreadSafeSet<C>::ThreadSafeSet() {
    sleepLockMilliseconds = std::chrono::milliseconds(5);
}

template<class C>
inline ThreadSafeSet<C>::ThreadSafeSet(int sLockMilliseconds) {
    if(sLockMilliseconds > 0)
        sleepLockMilliseconds = std::chrono::milliseconds(sLockMilliseconds);
    else
        sleepLockMilliseconds = std::chrono::milliseconds(1);
}

template<class C>
inline bool ThreadSafeSet<C>::contains(C data) {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    // If the set iterator is not equal to the end of the set, the set contains the data.
    bool temp = !(set.find(data) == set.end());
    return temp;
}

template<class C>
inline bool ThreadSafeSet<C>::safeInsert(C data) {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    bool temp = set.insert(data).second;
    return temp;
}

template<class C>
inline ThreadSafeSet<C>& ThreadSafeSet<C>::operator=(const ThreadSafeSet<C>& copy) {
    std::unique_lock<std::mutex> lock(mu, std::try_to_lock);
    while(!lock.owns_lock()) {
        std::this_thread::sleep_for(sleepLockMilliseconds);
        lock.try_lock();
    }
    set.clear();
    for(C element : copy.set) {
        set.insert(element);
    }
    return *this;
}

#endif