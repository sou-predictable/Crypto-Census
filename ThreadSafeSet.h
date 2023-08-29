#ifndef THREADSAFESET_H
#define THREADSAFESET_H

#include <mutex>
#include <thread>
#include <unordered_set>

/**
 * A wrapper class for std::set. This wrapper provides several thread-safe operations by using a mutex.
 */
template <class C>
class ThreadSafeSet {
    public:
        
        /**
         * Default constructor.
         * 
         * Initializes sleepLockMilliseconds, the number of miliseconds a thread should sleep after encountering a conflict
         * 
         * @tparam C the type of data the set stores. 
         */
        ThreadSafeSet();

        /**
         * Constructor.
         * 
         * Initializes sleepLockMilliseconds, the number of miliseconds a thread should sleep after encountering a conflict.
         * If the parameter passed is equal or less 0, sleepLockMiliseconds defaults to 1.
         * 
         * @tparam C the type of data the set stores. 
         * @param sLockMiliseconds the number of miliseconds a thread should sleep after encountering a conflict 
         */
        ThreadSafeSet(int sLockMilliseconds);

        /**
         * Thread safe wrapper around unordered_set.contains().
         * 
         * @tparam C the type of data the set stores.
         * @param data the object to search for.
         * @return true if the set contains the object, false otherwise.
         */
        bool contains(C data);

        /**
         * Thread safe wrapper around unordered_set.insert().
         * 
         * @tparam C the type of data the set stores.
         * @param data the object to insert.
         * @return true if the set contains the object, false otherwise.
         */
        bool safeInsert(C data);

        /**
         * Thread safe wrapper around set's copy constructor.
         * 
         * @tparam C the type of data the set stores.
         * @return a reference to the local variable.
         */
        ThreadSafeSet& operator=(const ThreadSafeSet& copy);

    private:
        std::mutex mu;
        std::unordered_set<C> set;
        std::chrono::milliseconds sleepLockMilliseconds;
};


template<class C>
inline ThreadSafeSet<C>::ThreadSafeSet() {
    sleepLockMilliseconds = std::chrono::milliseconds(1);
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