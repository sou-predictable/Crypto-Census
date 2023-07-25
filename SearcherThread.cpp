#include "OutputHandler.h"
#include "SearcherThread.h"
#include "TermMatcher.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeSet.h"
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_set>
/**
 * Constructor for SearcherThread
 * 
 * @param cIO the struct holding the input and output queue pointers for curl
 * @param killS a pointer to the kill switch semaphore
 * @param dQueue a pointer to a queue of domains to be searched
 * @param checkedDomains a pointer to a set of domains that have already been searched
 * @param sTerms the terms used by TermMatcher
 * @param eDomains excluded domains
 */
SearcherThread::SearcherThread(curlIO cIO, std::atomic<int>* killS, ThreadSafeQueue<std::string>* dQueue, ThreadSafeSet<std::string>* cDomains, Config* config) {
    curlOutputQueue = cIO.output;
    curlUrls = cIO.urls;
    killSwitch = killS;
    domainQueue = dQueue;
    checkedDomains = cDomains;
    o = OutputHandler();
    sleepLockMilliseconds = std::chrono::milliseconds(5);
}

/**
 * search sends domains to cURL, then takes the returned data and checks for potential terms.
 * 
 * If the site contains the needed number of terms, it is written to an output file.
 */
void SearcherThread::search(TermMatcher* validator) {
    // While the killswitch hasnt been thrown
    while(killSwitch->load() == 0) {
        if(!populateCurlQueue() && !consumeCurlQueue(validator))
            std::this_thread::sleep_for(sleepLockMilliseconds);
    }
    std::cout << "Searcher Exiting" << std::endl;
    o.close();
}

bool SearcherThread::populateCurlQueue() {
    std::string domainToCheck;

    if(domainQueue->empty())
        return false;
    while(domainQueue->safePop(&domainToCheck)) {
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
    while(curlOutputQueue->safePop(&curlOutput)) {
        // Check for the DOCTYPE decleration then write the domain if the site contains enough terms
        if(!curlOutput.siteContents.empty() && (curlOutput.siteContents.front().compare(0, htmlDoctypeTag.size(), htmlDoctypeTag)) == 0)
            if(validator && validator->matchTerms(curlOutput.siteContents, false))
                o.writeOutput(curlOutput.siteUrl);
    }
    return true;
}

//For now maybe limit based on output queue size? << gonna do a cIO setting