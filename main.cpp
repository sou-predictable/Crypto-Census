#include "DomainScraper.h"
#include "CurlHandler.h"
#include "TermMatcher.h"
#include <queue>
#include <string>
#include <fstream>
#include <unordered_set>

std::queue<std::string> getInitCrawlerQueue() {
    std::string currentLine;
    std::queue<std::string> initialCrawlerQueue;
    std::ifstream crawlerInputer = std::ifstream("sources.txt");
    while(std::getline(crawlerInputer, currentLine)) { initialCrawlerQueue.push("https://" + currentLine); }
    return initialCrawlerQueue;
}

std::unordered_set<std::string> getStringSetFromFile(std::string fileToRead) {
    std::string currentLine;
    std::unordered_set<std::string> set;
    std::ifstream exclusionInputer = std::ifstream(fileToRead);
    while(std::getline(exclusionInputer, currentLine)) { set.insert(currentLine); }
    return set;
}

int main() {
    std::unordered_set<std::string> exclusionSet = getStringSetFromFile("exclusions.txt");
    std::unordered_set<std::string> searchTermsSet = getStringSetFromFile("terms.txt");

    // Populate inputCrawlerQueue with domains found in "sources.txt"
    DomainScraper domain(getInitCrawlerQueue(), DomainScraper::CHECK_ALL_DOMAINS_FOR_TERMS);
    domain.setExclusions(getStringSetFromFile("exclusions.txt"));
    domain.setSearchTerms(getStringSetFromFile("terms.txt"));

    domain.startDomainScraper();

    return 0;
}