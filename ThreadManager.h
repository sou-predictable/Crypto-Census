#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include "Crawler.h"
#include "CurlThread.h"
#include "SearcherThread.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeSet.h"
#include "CurlInteractionStructs.h"
#include <atomic>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>

class ThreadManager {
    public:

        /**
         * ThreadManager constructor
         * 
         * The construction of this object automatically initiates the crawling and searching process.
         * This process manages all threads and waits for the user to submit a '\n' input before exiting.
         * 
         * @param iQueue the initial crawler queue.
         * @param eDomains the domains excluded from both the crawler and the searcher.
         */
        ThreadManager(ThreadSafeQueue<std::string>* iQueue, std::unordered_set<std::string> eDomains);

    private:
        Config config;

        TermMatcher validator;

        curlIO crawlerCurlIO;
        CurlThread crawlerCurl;
        std::thread crawlerCurlThread;
        ThreadSafeQueue<std::string> crawlerInitialQueue;

        std::thread crawlerThread;
        Crawler crawler;

        curlIO searcherCurlIO;
        CurlThread searcherCurl;
        std::thread searcherCurlThread;

        SearcherThread searcher;
        std::thread searcherThread;
        ThreadSafeQueue<std::string> extractedDomains;

        ThreadSafeSet<std::string> checkedDomains;

        std::unordered_set<std::string> searchTerms;
        std::unordered_set<std::string> excludedDomains;
        
        std::atomic<int> killSwitch;

        /**
         * verboseOutputThread contains logic for verbose output.
         * 
         * This output prints the input and output queues for the crawler's CURL thread and 
         * the searcher's CURL thread. The output is written every 100ms until the kill switch is thrown. 
         * 
         * @param verbose the killswitch for verbose output 
         */
        void verboseOutputThread(std::atomic<bool>* verbose);
};

#endif