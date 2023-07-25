#ifndef SEARCHERTHREAD_H
#define SEARCHERTHREAD_H

#include "Config.h"
#include "TermMatcher.h"
#include "ThreadSafeQueue.h"
#include "OutputHandler.h"
#include "ThreadSafeSet.h"
#include "CurlInteractionStructs.h"
#include <atomic>
#include <string>
#include <thread>
#include <unordered_set>

class SearcherThread {
    public:
        // SearcherThread constructors
        SearcherThread() = default;
        SearcherThread(curlIO cIO, std::atomic<int>* killS, ThreadSafeQueue<std::string>* dQueue, ThreadSafeSet<std::string>* cDomains, Config* config);

        /**
         * search sends domains to cURL, then takes the returned data and checks for potential terms.
         * 
         * If the site contains the needed number of terms, it is written to an output file.
         */
        void search(TermMatcher* validator);

    private:
        bool populateCurlQueue();
        bool consumeCurlQueue(TermMatcher* validator);
        ThreadSafeQueue<siteData>* curlOutputQueue;
        ThreadSafeQueue<std::string>* curlUrls;
        ThreadSafeQueue<std::string>* domainQueue;

        ThreadSafeSet<std::string>* checkedDomains;

        std::unordered_set<std::string> searchTerms;

        std::atomic<int>* killSwitch;

        OutputHandler o;
        
        std::chrono::milliseconds sleepLockMilliseconds;
};

#endif