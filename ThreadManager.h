#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include "Crawler.h"
#include "CurlThread.h"
#include "SearcherThread.h"
#include "OutputHandler.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeSet.h"
#include "CurlInteractionStructs.h"
#include <atomic>
#include <queue>
#include <thread>
#include <string>
#include <iostream>
#include <unordered_set>

class ThreadManager {
    public:
        // ThreadManager constructor
        ThreadManager(ThreadSafeQueue<std::string>* iQueue, std::unordered_set<std::string> eDomains);

        // appendToCrawlerQueue populates the crawler queue
        void populateInitialQueue(std::queue<std::string> newCrawlerQueue);
        void handleOutput(std::queue<std::string>* output);

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
};

#endif