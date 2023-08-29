#include "Crawler.h"
#include "Config.h"
#include "CurlInteractionStructs.h"
#include "TermMatcher.h"
#include "ThreadSafeSet.h"
#include "ThreadSafeQueue.h"
#include <atomic>
#include <boost/regex.hpp>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>
#include <unordered_map>

Crawler::Crawler(curlIO cIO, ThreadSafeQueue<std::string>* initialQueue, std::atomic<int>* killS, ThreadSafeQueue<std::string>* extractedDomains, std::unordered_set<std::string> eDomains, Config* config) {
    const int defaultMaxRequestsPerDomain = 150;
    const int defaultMaxExtractedLinksPerPage = 500;

    maxDomainSize = 253;  // https://www.freesoft.org/CIE/RFC/1035/9.htm

    sleepLockMilliseconds = std::chrono::milliseconds(20);
    
    curlOutputQueue = cIO.output;
    urlQueue = cIO.urls;
    killSwitch = killS;
    extractedDomainQueue = extractedDomains;
    excludedDomains = eDomains;

    maxRequestsPerDomain = config->getIntConfig("Crawler_MaxRequestsPerDomain", defaultMaxRequestsPerDomain);
    maxExtractedLinksPerPage = config->getIntConfig("Crawler_MaxExtractedLinksPerPage", defaultMaxExtractedLinksPerPage);
    
    std::string initialQueueData;
    if(initialQueue->empty())
        std::cout << "ERROR: sources.txt Has Not Been Populated\n";
    else
        while(initialQueue->safePop(&initialQueueData) && initialQueueData.length() < maxDomainSize) {
            std::queue<std::string> domain;
            if(initialQueueData.length() < maxDomainSize) {
                extractDomains(initialQueueData, &domain);
                if(!domain.empty())
                    queuedUrls.push(std::make_pair(initialQueueData, domain.front()));
                else
                    std::cout << "ERROR: Invalid Initial Domain: " << initialQueueData << "\n";
            } else
                std::cout << "ERROR: Initial Domain Exceeds Max Size: " << initialQueueData << "\n";
        }
        
    pushUrls();
}

void Crawler::crawl(TermMatcher* validator) {
    // Thread will operate until the killswitch is thrown 
    while(killSwitch->load() == 0) {
        siteData data;
        if(curlOutputQueue->safePop(&data)) {
            domainScraper(data, validator);
            pushUrls();
        // If there is no work, the crawler thread sleeps
        } else
            std::this_thread::sleep_for(sleepLockMilliseconds);
    }
    std::cout << "Crawler Exiting\n";
}

void Crawler::domainScraper(siteData inputData, TermMatcher* validator) {
    /**
     * Checks to see if there is any data, then checks to see if data returned is an HTML document.
     * If there is no data, or the document does start with the DOCTYPE decleration, the site is ignored.
     */ 
    const std::string htmlDoctypeTag = "<!DOCTYPE";
    if(inputData.siteContents.empty() || !(inputData.siteContents.front().compare(0, htmlDoctypeTag.size(), htmlDoctypeTag)) == 0)
        return;
    // If the site has the number of required terms, use regex to search for links and call extractDomains. Otherwise, the site is ignored
    if(validator->matchTerms(inputData.siteContents, false)) {
        const int urlIndex = 1;
        const int domainIndex = 3;
        const char selfReferencingLink = '/';
        const std::string defaultProtocol = "https://";

        // processSiteContents is called on the site contents to parse out potential domains
        processSiteContents(inputData);

        // This regex idientifies A HREF links for the crawler to follow
        // This regex ignores URL fragments by truncating the fragment part of the URL
        boost::regex expression(R"(<a[^h>]*href=\"(([^:\/]*:\/\/([^\/":?]+)|\/)[^"&=>#?]*))");

        std::queue<std::string> siteDomain;
        
        // Find current URL's domain
        extractDomains(inputData.siteUrl, &siteDomain);
        
        int linksFound = 0;
        // Limit total links processed per page
        for(size_t i = 0; i < inputData.siteContents.size() && linksFound < maxExtractedLinksPerPage; i++) {
            boost::sregex_iterator j = boost::sregex_iterator(inputData.siteContents[i].begin(), inputData.siteContents[i].end(), expression);
            boost::sregex_iterator end;
            // Iterate through every found link
            for(; j != end && linksFound < maxExtractedLinksPerPage; j++, linksFound++) {
                boost::smatch match = *j;
                /** 
                 * Handle self referencing links by concatenating current_url to just the domain, then adding the rest of the match
                 * If the root domain could not be extracted, ignore the self referencing URL
                 */
                if(match.str(urlIndex)[0] == selfReferencingLink && !siteDomain.empty())
                    // Send URL for validation
                    queuedUrls.push(std::make_pair(defaultProtocol + siteDomain.front() + match.str(urlIndex), siteDomain.front()));
                // Exclude excluded domains
                else if(excludedDomains.empty() || excludedDomains.find(match.str(domainIndex)) == excludedDomains.end())
                    queuedUrls.push(std::make_pair(match.str(urlIndex), match.str(domainIndex)));
            }
        }
    }
}

void Crawler::extractDomains(std::string data, std::queue<std::string>* extractedDomains) {
    // This expression is used to filter for domains
    boost::regex expression(R"([^\w\.\-]([\w-]+?\.(([\w-]+?\.)+)?([a-zA-Z]+|XN--[A-Za-z0-9]+)))");

    boost::sregex_iterator i = boost::sregex_iterator(data.begin(), data.end(), expression);
    boost::sregex_iterator end;

    /**
     * For every domain extracted, check for a valid top level domain, then exclude any matches
     * that are followed by a `(` character to reduce false positives
     */
    for(; i != end; i++) {
        boost::smatch match = *i;
        
        if(TOP_LEVEL_DOMAINS.find(match.str(4)) != TOP_LEVEL_DOMAINS.end() && ((std::string)match.suffix())[1] != '(')
            extractedDomains->push(match.str(1));
    }
}

void Crawler::extractDomains(std::vector<std::string> data, std::queue<std::string>* extractedDomains) {
    // For every string in the data
    for(std::string it : data) {
        // Perform domain extraction on the string
        extractDomains(it, extractedDomains);
    }
}

void Crawler::processSiteContents(siteData inputData) {
    std::queue<std::string> extractedDomains;
    extractDomains(inputData.siteContents, &extractedDomains);        
    
    while(!extractedDomains.empty()) {
        // Excludes domains on the exclusion list, and domains that are too large
        if(extractedDomains.front().size() < maxDomainSize && excludedDomains.find(extractedDomains.front()) == excludedDomains.end())
            extractedDomainQueue->push(extractedDomains.front());
        extractedDomains.pop();
    }
}

void Crawler::pushUrls() {
    while(!queuedUrls.empty()) {
        std::string url = queuedUrls.front().first;
        std::string domain = queuedUrls.front().second;

        if(!traversedDomains.contains(domain) && url.length() > 0) {
            // If this URL's domain has never been seen before, create a new domain entry
            if(visitedUrlsPerDomain.find(domain) == visitedUrlsPerDomain.end()) {
                std::unordered_set<std::string> newSet = {url};
                visitedUrlsPerDomain.insert(std::make_pair(domain, newSet));
                urlQueue->push(url);
            // If the domain has been seen, and the URL has not been visited before
            } else if(visitedUrlsPerDomain[domain].find(url) == visitedUrlsPerDomain[domain].end()) {
                // If the number of URLs visited meets the MAX_LINKS_PER_DOMAIN after this addition
                if(visitedUrlsPerDomain[domain].size() >= maxRequestsPerDomain) {
                    // Exclude URLs associated with this domain in the future
                    traversedDomains.safeInsert(domain);
                    // Save memory by deleting the URLs stored in the map
                    visitedUrlsPerDomain.erase(visitedUrlsPerDomain.find(domain));
                } else
                    visitedUrlsPerDomain[domain].insert(url);
                urlQueue->push(url);
            }
        }
        queuedUrls.pop();
    }
}