#include "Config.h"
#include "ThreadManager.h"
#include "ThreadSafeQueue.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

/**
 * getInitCrawlerQueue gets the initial crawler queue from a text file.
 * 
 * @param fileToRead the file to read.
 * @return the crawler queue built from the source file.
 */
void populateInitCrawlerQueue(std::string fileToRead, ThreadSafeQueue<std::string>* initialCrawlerQueue) {
    const std::string defaultProtocol = "https://";
    std::string currentLine;
    std::ifstream crawlerInputer = std::ifstream(fileToRead);
    while(std::getline(crawlerInputer, currentLine)) {
        initialCrawlerQueue->push(defaultProtocol + currentLine);
    }
}

/**
 * getStringSetFromFile builds a string set from a text file.
 * 
 * @param fileToRead the file to read.
 * @return the string set built from the provided file.
 */
std::unordered_set<std::string> getStringSetFromFile(std::string fileToRead) {
    std::string currentLine;
    std::unordered_set<std::string> set;
    std::ifstream exclusionInputer = std::ifstream(fileToRead);
    while(std::getline(exclusionInputer, currentLine)) {
        set.insert(currentLine);
    }
    return set;
}

/**
 * CryptoCensus works by utilizing a crawler/scraper to identify crypto-related domains.
 * 
 * The program does this by using a list of terms commonly found on crypto-related sites.
 * If a webpage contains a number of unique crypto terms, the page is classified as crypto-related.
 * The program then attempts to check the webpage's domain for crypto sites.
 * If the webpage's domain also contains enough terms to be classified as a crypto-related site, the domain is flagged as crypto-related.
 * 
 * Additionally, any domains found on the webpage are also checked to see if the domain is crypto related.
 * 
 * The domain checks are done to exclude sites that may discuss crypto activity, but are not devoted to crypto (such as wikipedia and youtube).
 * 
 * This program requires the following files in the same directory as the executable:
 * sources.txt - the initial sites to start crawling from
 * terms.txt - the terms to search for
 * exclusions.txt - the exclusions
 * cacert.pem - the CA CERT
 * libcurl-x64.dll - the libcurl library
 */
int main(int argc, char** argv) {
    Config c;
    curl_global_init(CURL_GLOBAL_ALL);

std::string logo = R"(
  _____  ____ __   __ ____ _____ _____     _____  _____ _   _   ____ _   _  ____ 
 /  __ \| ___ \ \ / / ___ \_   _|  _  |   /  __ \|  ___| \ | |/  ___| | | /  ___|
|  /  \/| |_/ /\ V /| |_/ / | | | | | |  |  /  \/| |__ |  \| |\ `--.| | | \ `--. 
| |     |    /  \ / |  __/  | | | | | |  | |     |  __|| . ` | `--. \ | | |`--. \
|  \__/\| |\ \  | | | |     | | \ \_/ /  |  \__/\| |___| |\  |/\__/ / |_| /\__/ /
 \_____/\_| \_| \_| \_|     \_|  \___/    \_____/\____/\_| \_|\____/ \___/\____/ )";

    std::cout << logo << "\n\n";

    // Populate inputCrawlerQueue with domains found in "sources.txt"

    ThreadSafeQueue<std::string> initialCrawlerQueue;
    populateInitCrawlerQueue("sources.txt", &initialCrawlerQueue);
    ThreadManager manager(&initialCrawlerQueue, getStringSetFromFile("exclusions.txt"));
    curl_global_cleanup();
    return 0;
}