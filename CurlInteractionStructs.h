#ifndef SITEDATASTRUCT_H
#define SITEDATASTRUCT_H

#include "ThreadSafeQueue.h"
#include <vector>
#include <string>

struct siteData {
    std::vector<std::string> siteContents;
    std::string siteUrl;

    siteData() = default;

    siteData(std::vector<std::string> contents, std::string url) {
        siteContents = contents;
        siteUrl = url;
    }
};

struct curlIO {
    ThreadSafeQueue<siteData>* output;
    ThreadSafeQueue<std::string>* urls;
    int maxConnections;
    int maxOutputQueueSize;
};

#endif