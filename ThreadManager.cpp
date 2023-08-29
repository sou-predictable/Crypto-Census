#include "ThreadManager.h"
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

ThreadManager::ThreadManager(ThreadSafeQueue<std::string>* iQueue, std::unordered_set<std::string> eDomains) {
    const int defaultCrawlerMaxConnections = 500;
    const int defaultSearcherMaxConnections = 1000;

    config = Config();

    validator = TermMatcher(&config);

    // Initialize the excluded domains
    excludedDomains = eDomains;

    // Create crawler curl IO queues
    crawlerCurlIO.output = new ThreadSafeQueue<siteData>();
    crawlerCurlIO.urls = new ThreadSafeQueue<std::string>();
    crawlerCurlIO.maxConnections = config.getIntConfig("Crawler_MaxConnections", defaultCrawlerMaxConnections);
    // Create searcher curl IO queues
    searcherCurlIO.output = new ThreadSafeQueue<siteData>();
    searcherCurlIO.urls = new ThreadSafeQueue<std::string>();
    searcherCurlIO.maxConnections = config.getIntConfig("Searcher_MaxConnections", defaultSearcherMaxConnections);


    killSwitch = 0;

    // Create the crawler thread and crawler object
    crawler = Crawler(crawlerCurlIO, iQueue, &killSwitch, &extractedDomains, excludedDomains, &config);
    crawlerThread = std::thread(&Crawler::crawl, &crawler, &validator);

    // Create the searcher thread and searcher object
    searcher = SearcherThread(searcherCurlIO, &killSwitch, &extractedDomains, &checkedDomains, &config);
    searcherThread = std::thread(&SearcherThread::search, &searcher, &validator);    


    // Create the crawler curl thread and curl object
    crawlerCurl = CurlThread(crawlerCurlIO, &killSwitch, &config);
    crawlerCurlThread = std::thread(&CurlThread::consumeUrls, &crawlerCurl);


    // Create the searcher curl thread and curl object
    searcherCurl = CurlThread(searcherCurlIO, &killSwitch, &config);
    searcherCurlThread = std::thread(&CurlThread::consumeUrls, &searcherCurl);

    std::thread verboseThread;

    std::atomic<bool> verbose;

    // Wait for manual termination
    char userInput;
    do {
        std::cout << "\n================== Silent Mode ===================\nEnter 'v' for verbose real-time updates.\nEnter 'e' to exit." << std::endl;

        userInput = std::cin.get();
        if(userInput == 'v') {
            // Ignore the newline character from the 'v' submission
            std::cin.get();

            verbose.store(true);
            verboseThread = std::thread(ThreadManager::verboseOutputThread, this, &verbose);

            do {} while(std::cin.get() != '\n');
            
            verbose.store(false);
            verboseThread.join();
        }
    } while(userInput != 'e');

    // Flip the killswitch
    killSwitch++;

    // Join all threads
    crawlerThread.join();
    crawlerCurlThread.join();

    searcherThread.join();
    searcherCurlThread.join();
}

void ThreadManager::verboseOutputThread(std::atomic<bool>* verbose) {
    std::cout << "\n================== Verbose Mode ==================\nHit 'Enter' to return to silent mode.\n\n";
    // This prints about 30 frames per second
    const std::chrono::milliseconds outputRefreshRate = std::chrono::milliseconds(50);
    while(verbose->load() == true) {
        std::cout << "\rCrawler - Queued Sites: " << crawlerCurlIO.urls->size() 
                  << " - Processing: " << crawlerCurlIO.output->size()
                  << " | Validator - Queued Sites: " << searcherCurlIO.urls->size()
                  << " - Processing: " << searcherCurlIO.output->size()
                  << "    ";
        std::this_thread::sleep_for(outputRefreshRate);
    }
}