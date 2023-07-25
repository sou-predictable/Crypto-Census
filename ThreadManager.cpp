#include "ThreadManager.h"
#include "Crawler.h"
#include "CurlThread.h"
#include "SearcherThread.h"
#include "OutputHandler.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeSet.h"
#include "CurlInteractionStructs.h"
#include <queue>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

/**
 * ThreadManager constructor
 * 
 * @param iQueue the initial crawler queue
 * @param sTerms the search terms to use
 * @param eDomains the domains excluded from both the crawler and the searcher
 */
ThreadManager::ThreadManager(ThreadSafeQueue<std::string>* iQueue, std::unordered_set<std::string> eDomains) {
    const int defaultCrawlerMaxConnections = 100;
    const int defaultSearcherMaxConnections = 100;

    config = Config();

    std::cout << "Press the 'Enter' key to exit" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    validator = TermMatcher(&config);

    // Initialize the excluded domains
    excludedDomains = eDomains;

    // Create crawler cURL IO queues
    crawlerCurlIO.output = new ThreadSafeQueue<siteData>();
    crawlerCurlIO.urls = new ThreadSafeQueue<std::string>();
    crawlerCurlIO.maxConnections = config.getIntConfig("Crawler_MaxConnections", defaultCrawlerMaxConnections);
    // Create searcher cURL IO queues
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


    // Create the crawler cURL thread and cURL object
    crawlerCurl = CurlThread(crawlerCurlIO, &killSwitch, &config);
    crawlerCurlThread = std::thread(&CurlThread::consumeUrls, &crawlerCurl);


    // Create the searcher cURL thread and cURL object
    searcherCurl = CurlThread(searcherCurlIO, &killSwitch, &config);
    searcherCurlThread = std::thread(&CurlThread::consumeUrls, &searcherCurl);

    // Wait for manual termination
    std::string exit;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } while(std::cin.get() != '\n');

    // Flip the killswitch
    killSwitch++;

    // Join all threads
    crawlerThread.join();
    crawlerCurlThread.join();

    searcherThread.join();
    searcherCurlThread.join();

    // Cleanup the cURL messes
    crawlerCurl.cleanup();
    searcherCurl.cleanup();
}