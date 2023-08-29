#ifndef SITEDATASTRUCT_H
#define SITEDATASTRUCT_H

#include "ThreadSafeQueue.h"
#include <vector>
#include <string>

/**
 * A structure representing site data.
 * 
 * This struct holds both a vector of strings representing the site's data, and a string representing the site's URL
 */
struct siteData {
    std::vector<std::string> siteContents;
    std::string siteUrl;
    int maxContentBytes;

    siteData() = default;

    siteData(std::vector<std::string> contents, std::string url) {
        siteContents = contents;
        siteUrl = url;
    }
};

// A structure representing input/output elements and configurations needed for the curl handler. 
struct curlIO {
    ThreadSafeQueue<siteData>* output;
    ThreadSafeQueue<std::string>* urls;
    int maxConnections;
    int maxOutputQueueSize;
};

#endif