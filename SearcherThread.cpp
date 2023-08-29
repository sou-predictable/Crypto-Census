#include "SearcherThread.h"
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

SearcherThread::SearcherThread(curlIO cIO, std::atomic<int>* killS, ThreadSafeQueue<std::string>* dQueue, ThreadSafeSet<std::string>* cDomains, Config* config) {
    curlOutputQueue = cIO.output;
    curlUrls = cIO.urls;
    killSwitch = killS;
    domainQueue = dQueue;
    checkedDomains = cDomains;
    sleepLockMilliseconds = std::chrono::milliseconds(20);
    output = std::ofstream("output.txt", std::ofstream::out);
}

void SearcherThread::search(TermMatcher* validator) {
    // While the killswitch hasnt been thrown
    while(killSwitch->load() == 0) {
        if(!pushToCurlQueue() && !consumeCurlQueue(validator))
            std::this_thread::sleep_for(sleepLockMilliseconds);
    }
    std::cout << "Searcher Exiting\n";
    output.close();
}

bool SearcherThread::pushToCurlQueue() {
    std::string domainToCheck;

    if(domainQueue->empty())
        return false;
    if(domainQueue->safePop(&domainToCheck)) {
        // If the domain has not been visited yet, push these elements to the curlUrls queue
        if(checkedDomains->safeInsert(domainToCheck))
            curlUrls->push(domainToCheck);
    }
    return true;
}

bool SearcherThread::consumeCurlQueue(TermMatcher* validator) {
    const std::string htmlDoctypeTag  = "<!DOCTYPE";

    siteData curlOutput;

    if(curlOutputQueue->empty())
        return false;
    if(curlOutputQueue->safePop(&curlOutput)) {
        // Check for the DOCTYPE decleration then write the domain if the site contains enough terms
        if(!curlOutput.siteContents.empty() && (curlOutput.siteContents.front().compare(0, htmlDoctypeTag.size(), htmlDoctypeTag)) == 0)
            if(validator && validator->matchTerms(curlOutput.siteContents, false))
                output << curlOutput.siteUrl << std::endl;
    }
    return true;
}

//For now maybe limit based on output queue size? << gonna do a cIO setting