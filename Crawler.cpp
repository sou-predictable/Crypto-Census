#include "Crawler.h"
#include "CurlHandler.h"
#include <queue>
#include <string>
#include <unordered_set>

// Default constructor
Crawler::Crawler() { crawlerQueue = std::queue<std::string>(); }

/**
 * @overload the default constructor
 * @param newCrawlerQueue the initial crawler queue
 */
Crawler::Crawler(std::queue<std::string> newCrawlerQueue) { appendToCrawlerQueue(newCrawlerQueue); }

/** 
 * crawl is a crawler implementation which uses CurlHandler to fetch website data. This data is passed to a callback function for further manipulation and link extraction.  
 * The callback function must then return a queue of string representations of URLs for the crawler to continue trawling.
 * @param crawlerCallbackFunction a callback function which takes a string URL and string website data returned by CURL and returns a queue of URLs to continue with. 
 */ 
void Crawler::crawl(std::queue<std::string> (*crawlerCallbackFunction)(std::string current_url, std::string website_data)) {
    while(!crawlerQueue.empty()) {
        std::queue<std::string> urlsToAppend = crawlerCallbackFunction(crawlerQueue.front(), curlh.curlHttpsSite(crawlerQueue.front()));
        crawlerQueue.pop();
        // Iterates through the URLs the callback function is attempting to add to the crawler queue.
        while(!urlsToAppend.empty()) {
            // Check if insertion occurred. If no insertion was performed, the front URL in urlsToAppend has already been crawled and will be ignored.
            if(traversedUrls.insert(urlsToAppend.front()).second)
                crawlerQueue.push(urlsToAppend.front());
            urlsToAppend.pop();
        }
    }
}

void Crawler::appendToCrawlerQueue(std::queue<std::string> newCrawlerQueue) {    
    while(!newCrawlerQueue.empty()) {
        if(traversedUrls.insert(newCrawlerQueue.front()).second)
            crawlerQueue.push(newCrawlerQueue.front());
        newCrawlerQueue.pop();
    }
}