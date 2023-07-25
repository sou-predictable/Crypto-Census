#ifndef CURLTHREAD_H
#define CURLTHREAD_H

/**
 * @file CurlThread.h
 *
 * @brief Implementation of libcurl which takes a string representation of a URL and points cURL at the specified URL. The output is then handled and returned as a string.
 * 
 * @author Alexander Sou
 * 
 * @version 0.1.0
 * 
 */

#include "Config.h"
#include "CurlInteractionStructs.h"
#include "ThreadSafeQueue.h"
#include <atomic>
#include <curl/curl.h>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
/**
 * Implementation of libcurl.
 * 
 * CurlThread takes a string representation of a URL and points cURL at the specified URL. The output is then handled and returned as a string.
 */
class CurlThread {
    public:
        // CurlThread constructors
        CurlThread() = default;
        CurlThread(curlIO cIO, std::atomic<int>* kSwitch, Config* config);

        // cleanup performs the libcurl cleanup operations
        void cleanup();

        /**
         * consumeUrls uses libcurl's curl_multi_perform to run multiple simultaneous transfers.
         * curl_multi_poll is then called to wait for activity from any of the ongoing transfers.
         * curl_multi_info_read is called to read what happened to transfers that have completed.
         * 
         * Libcurl easy handles which have completed their transfers are placed in the handlesWaitingForNewURLs
         * queue to await additional URLs.
         */
        void consumeUrls();
    private:
        CURLM* multiHandle;

        std::queue<CURL*> handlesWaitingForNewURLs;

        ThreadSafeQueue<std::string>* urlQueue;
        ThreadSafeQueue<siteData>* outputQueue;

        std::atomic<int>* killSwitch;

        std::unordered_map<CURL*, siteData*> easyHandles;

        /**
         * Static callback class which follows the prototype found at https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
         * This function is used by the libcurl Easy Interface curl_easy_setopt function with the CURLOPT_WRITEFUNCTION option
         */
        static size_t curlWriteDataCallback(char* ptr, size_t size, size_t nmemb, siteData* buffer);

        /**
         * updateHandleURL updates the easy handle with a new URL
         * The easy handle is removed and re-added to the multi handle to update the values
         */
        void updateHandleURL(CURL* eHandle, std::string url);
        void updateHandles(int* handlesRunning);

        struct curl_slist* HTTPHeaderOptions;

        std::chrono::milliseconds sleepLockMilliseconds;
};

#endif