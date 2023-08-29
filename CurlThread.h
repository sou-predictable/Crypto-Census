#ifndef CURLTHREAD_H
#define CURLTHREAD_H

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
 * CurlThread takes a string representation of a URL and points curl at the specified URL. The output is then handled and returned as a string.
 */
class CurlThread {
    public:

        // Default constructor.
        CurlThread() = default;

        /**
         * Constructor.
         * 
         * @param cIO the struct holding the input and output queue pointers for curl.
         * @param kSwitch a pointer to the kill switch semaphore.
         * @param config a struct which holds the configurations for the CurlThread.
         *      CurlThread Configs:
         *          Curl_SslCertLocation the location of the SSL cert. Defaults to "cacert.pem".
         *          Curl_UserAgent the user agent used with every connection. Defaults to a Chrome user agent.
         *          Curl_BytesToRead the number of bytes to read.
         *          Curl_MaxRedirects the maximum number of redirects to follow in the case of a 3XX response code.
         *          Curl_Timeout the time CurlThread allots to an exceptionally slow connection before the connection is closed.
         */
        CurlThread(curlIO cIO, std::atomic<int>* kSwitch, Config* config);

        // Performs the libcurl cleanup operations.
        void cleanup();

        /**
         * Uses libcurl's curl_multi_perform to run multiple simultaneous transfers.
         * 
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

        struct curl_slist* HTTPHeaderOptions;

        std::chrono::milliseconds sleepLockMilliseconds;

        int upperByteLimit;
        /**
         * Static callback class which follows the prototype found at https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html.
         * This function is used by the libcurl Easy Interface curl_easy_setopt function with the CURLOPT_WRITEFUNCTION option.
         * 
         * @param[in] ptr a pointer to the data delivered by curl.
         * @param[in] size "size is always 1" (https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html).
         * @param[in] nmemb the size of the data delivered by curl.
         * @param[out] buffer the siteData object output is written to.
         */
        static size_t curlWriteDataCallback(char* ptr, size_t size, size_t nmemb, siteData* buffer);

        
        /**
         * updateHandleURL updates the easy handle with a new URL.
         * 
         * The easy handle is removed and re-added to the multi handle to update the values.
         * 
         * @param eHandle the handle to refresh.
         * @param url the URL the handle should query next.
         */
        void updateHandleURL(CURL* eHandle, std::string url);
};

#endif