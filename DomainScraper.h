#ifndef DOMAINSCRAPER_H
#define DOMAINSCRAPER_H

#include "TermMatcher.h"
#include "Crawler.h"
#include <queue>
#include <regex>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_set>

class DomainScraper {
    public:
        static const int CHECK_ALL_DOMAINS_FOR_TERMS = 1;
        
        DomainScraper(std::queue<std::string> iCrawlerQueue);
        DomainScraper(std::queue<std::string> iCrawlerQueue, int options);
        
        void startDomainScraper();
        static std::string getOutputFile();
        static void setSearchTerms(std::unordered_set<std::string> searchTerms);
        static void setExclusions(std::unordered_set<std::string> exclusions);

    private:
        Crawler domainCrawler;

        static const int REQUIRED_UNIQUE_TERMS = 4;
        static const std::string OUTPUT_FILENAME;
        static Crawler termChecker;
        static bool check_all_domains_for_terms;
        static std::ofstream outputStream;
        static std::unordered_set<std::string> searchTerms;
        static std::unordered_set<std::string> excludedDomains;
        static std::unordered_set<std::string> traversedDomains;

        static void writeFoundDomain(std::string foundDomain);
        static std::queue<std::string> domainScraperCallbackFunction(std::string currentUrl, std::string websiteData);
        static std::queue<std::string> checkSiteForTerms(std::string currentUrl, std::string websiteData);
        static std::queue<std::string> extractDomains(std::string data);
        static std::string validateWebsiteData(std::string websiteData);
};

#endif