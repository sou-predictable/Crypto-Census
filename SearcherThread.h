#ifndef SEARCHERTHREAD_H
#define SEARCHERTHREAD_H

#include "Config.h"
#include "CurlInteractionStructs.h"
#include "TermMatcher.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeSet.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_set>

class SearcherThread {
    public:

        // Default constructor
        SearcherThread() = default;

        /**
         * Constructor for SearcherThread
         * 
         * @param cIO the struct holding the input and output queue pointers for curl.
         * @param killS a pointer to the kill switch semaphore.
         * @param dQueue a pointer to a queue of domains to be searched.
         * @param cDomains a pointer to domains already checked.
         * @param config a pointer to the object holding the program's configurations.
         */
        SearcherThread(curlIO cIO, std::atomic<int>* killS, ThreadSafeQueue<std::string>* dQueue, ThreadSafeSet<std::string>* cDomains, Config* config);

        /**
         * Uses curl to check subdomain homepages for terms.
         * 
         * If the subdomain contains a specified number of terms, it is written to an output file.
         * 
         * @param validator the TermMatcher object to use during domain validation.
         */
        void search(TermMatcher* validator);

    private:
        ThreadSafeQueue<siteData>* curlOutputQueue;
        ThreadSafeQueue<std::string>* curlUrls;
        ThreadSafeQueue<std::string>* domainQueue;

        ThreadSafeSet<std::string>* checkedDomains;

        std::unordered_set<std::string> searchTerms;

        std::atomic<int>* killSwitch;

        std::ofstream output;
        
        std::chrono::milliseconds sleepLockMilliseconds;

        /**
         * Checks against the domain at the front of the domainQueue against the list of checkedDomains.
         * 
         * If the domain hasn't been scraped by SearcherThread before, the domain is pushed to the curl
         * input queue and is added to the checkedDomains set.
         * 
         * @return true if the domain is pushed to the curl's input queue. Returns false otherwise.  
         */
        bool pushToCurlQueue();
        
        /**
         * Passes a site contents as a vector of strings to TermMatcher to determine whether a domain meets certain criteria. 
         * 
         * If it does, TermMatcher returns true, and the site's url is sent to the output file.
         * 
         * @param validator the TermMatcher to use in the domain validation process.
         * @return false if the curlOutputQueue is empty, true otherwise.
         */
        bool consumeCurlQueue(TermMatcher* validator);
};

#endif