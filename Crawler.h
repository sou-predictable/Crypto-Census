#ifndef CRAWLER_H
#define CRAWLER_H

#include "CurlHandler.h"
#include <queue>
#include <string>
#include <unordered_set>

class Crawler {
    public:
        Crawler();
        Crawler(std::queue<std::string> newCrawlerQueue);

        /** 
         * crawl is a crawler implementation which uses CurlHandler to fetch website data. This data is passed to a callback function for further manipulation and link extraction.  
         * The callback function must then return a queue of string representations of URLs for the crawler to continue trawling.
         * @param crawlerCallbackFunction a callback function which takes a string URL and string website data returned by CURL and returns a queue of URLs to continue with. 
         */ 
        void crawl(std::queue<std::string> (* crawlerCallbackFunction)(std::string current_url, std::string website_data));
        void appendToCrawlerQueue(std::queue<std::string> newCrawlerQueue);

    private:
        CurlHandler curlh;
        std::queue<std::string> crawlerQueue;
        std::unordered_set<std::string> traversedUrls;
};

#endif