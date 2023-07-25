#include "CurlThread.h"
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
 * CurlThread constructor
 * 
 * @param cIO the struct holding the input and output queue pointers for curl
 * @param killS a pointer to the kill switch semaphore
 */
CurlThread::CurlThread(curlIO cIO, std::atomic<int>* kSwitch, Config* config) {
    // Non-configurable libcurl constants
    const std::string acceptedProtocols = "http,https";
    const std::string preferredProtocol = "https";
    const std::string lowerByteRange = "0-";                        // See https://curl.se/libcurl/c/CURLOPT_RANGE.html
    const long slowTimeoutBytesASecond = 50L;                       // See https://curl.se/libcurl/c/CURLOPT_LOW_SPEED_LIMIT.html
    const long hardTimeOutSeconds = 300L;                           // Hard timeout in seconds
    const long enableRedirects = 1L;
    const long enableNoSignal = 1L;                                 // See https://curl.se/libcurl/c/threadsafe.html

    const std::string defaultSslCertLocation = "cacert.pem";        // The location of the SSL cert
    const std::string defaultUserAgent = "Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
    const int defaultUpperByteRange = 15000000;                     // Number of bytes to read per page
    
    const long defaultSlowTimeoutSeconds = 5L;                      // See https://curl.se/libcurl/c/CURLOPT_TIMEOUT.html
    const long defaultMaxRedirects = 3L;
    const int defaultMaxConnections = 10;

    const std::chrono::milliseconds sLockMilliseconds = std::chrono::milliseconds(100);
    
    sleepLockMilliseconds = sLockMilliseconds;

    outputQueue = cIO.output;
    urlQueue = cIO.urls;

    int MaxConnections = defaultMaxConnections;
    if(cIO.maxConnections > 0)
        MaxConnections = cIO.maxConnections;

    const std::string sslCertLocation = config->getConfig("Curl_SslCertLocation", defaultSslCertLocation);
    const std::string userAgent = config->getConfig("Curl_UserAgent", defaultUserAgent);
    const std::string acceptedByteRange = lowerByteRange + std::to_string(config->getIntConfig("Curl_BytesToRead", defaultUpperByteRange));
    const long maxRedirects = config->getLongConfig("Curl_MaxRedirects", defaultMaxRedirects);
    const long slowTimeoutSeconds = config->getLongConfig("Curl_Timeout", defaultSlowTimeoutSeconds);

    killSwitch = kSwitch;
    
    multiHandle = curl_multi_init();
    curl_multi_setopt(multiHandle, CURLMOPT_MAX_HOST_CONNECTIONS, 50);

    HTTPHeaderOptions = NULL;
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Accept-Encoding: ");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Accept-Language: en-US,en;q=0.9");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Sec-Ch-Ua: \"Not.A/Brand\";v=\"8\", \"Chromium\";v=\"114\", \"Google Chrome\";v=\"114\"");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Sec-Ch-Ua-Mobile: ?0");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Sec-Ch-Ua-Platform: \"Windows\"");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Sec-Fetch-Dest: document");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Sec-Fetch-Mode: navigate");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Sec-Fetch-Site: same-origin");
    HTTPHeaderOptions = curl_slist_append(HTTPHeaderOptions, "Sec-Fetch-User: ?1");

    // For each allowed connection
    for(size_t i = 0; i < MaxConnections; i++) {
        // Initialize the cURL easy handle
        CURL* eHandle = curl_easy_init();
        // Checks to see if cURL is valid and proceeds with the setup and if it is
        if(eHandle) {
            siteData* sData = new siteData();               // A siteData object the callback function uses to output data

            // Calls the static callback function curlCallback
            curl_easy_setopt(eHandle, CURLOPT_WRITEFUNCTION, CurlThread::curlWriteDataCallback);

            curl_easy_setopt(eHandle, CURLOPT_WRITEDATA, sData);
            curl_easy_setopt(eHandle, CURLOPT_FOLLOWLOCATION, enableRedirects);
            curl_easy_setopt(eHandle, CURLOPT_REDIR_PROTOCOLS_STR, acceptedProtocols.c_str());
            curl_easy_setopt(eHandle, CURLOPT_PROTOCOLS_STR, acceptedProtocols.c_str());
            curl_easy_setopt(eHandle, CURLOPT_DEFAULT_PROTOCOL, preferredProtocol.c_str());
            curl_easy_setopt(eHandle, CURLOPT_LOW_SPEED_TIME, slowTimeoutSeconds);
            curl_easy_setopt(eHandle, CURLOPT_LOW_SPEED_LIMIT, slowTimeoutBytesASecond);
            curl_easy_setopt(eHandle, CURLOPT_TIMEOUT, hardTimeOutSeconds);
            curl_easy_setopt(eHandle, CURLOPT_NOSIGNAL, enableNoSignal);
            curl_easy_setopt(eHandle, CURLOPT_CAINFO, sslCertLocation.c_str());
            curl_easy_setopt(eHandle, CURLOPT_USERAGENT, userAgent.c_str());
            curl_easy_setopt(eHandle, CURLOPT_MAXREDIRS, maxRedirects);
            curl_easy_setopt(eHandle, CURLOPT_RANGE, acceptedByteRange);
            curl_easy_setopt(eHandle, CURLOPT_HTTPHEADER, HTTPHeaderOptions);
            handlesWaitingForNewURLs.push(eHandle);

            easyHandles.insert(std::pair<CURL*, siteData*> (eHandle, sData));
        } else {
            // Todo: Error Handling
            std::cout << "Error Initializing EasyHandle" << std::endl;
        }
    }
}

/**
 * cleanup performs the libcurl cleanup operations
 */
void CurlThread::cleanup() {
    curl_slist_free_all(HTTPHeaderOptions);
    for(auto handle : easyHandles) {
        curl_multi_remove_handle(multiHandle, handle.first);
        if(handle.first)
            curl_easy_cleanup(handle.first);
    }
    curl_multi_cleanup(multiHandle);
}

/**
 * consumeUrls uses libcurl's curl_multi_perform to run multiple simultaneous transfers.
 * curl_multi_poll is then called to wait for activity from any of the ongoing transfers.
 * curl_multi_info_read is called to read what happened to transfers that have completed.
 * 
 * Libcurl easy handles which have completed their transfers are placed in the handlesWaitingForNewURLs
 * queue to await additional URLs.
 */
void CurlThread::consumeUrls() {
    int isRunning;
    struct CURLMsg* message;
    CURLMcode multiResponse;
    std::string url;
    // While the kill switch has not been thrown
    do {
        int messageQueueItems = 0;
        int messagesRead = 0;
        bool workDone = false;
        
        // While the URL queue is not empty, and there exists handles waiting for new URLs
        while(!handlesWaitingForNewURLs.empty() && urlQueue->safePop(&url)) {
            // Call updateHandleUrl, and provide it with the first waiting handle, and the first waiting URL
            updateHandleURL(handlesWaitingForNewURLs.front(), url);

            handlesWaitingForNewURLs.pop();
            
            workDone = true;
        }

        // Execute all transfers pending further execution
        multiResponse = curl_multi_perform(multiHandle, &isRunning);
        // If execution was successful
        if(multiResponse != CURLM_OK) {
            std::cout << "CURL MULTI ERROR: " << multiResponse << std::endl;
            break;
        }

        // Read messages from CURL's message queue
        message = curl_multi_info_read(multiHandle, &messageQueueItems);
        // While there are finished transfers
        while(message && messagesRead < 100) {
            messagesRead++;
            // If the message exists
            if(message) {
                CURL* eHandle = message->easy_handle;
                // If the message indicates a completed transfer
                if(message->msg == CURLMSG_DONE) {
                    // If the transfer was successful
                    if(message->data.result == CURLE_OK) {
                        // Find the siteData the curl handle used
                        std::unordered_map<CURL*, siteData*>::iterator outputIt = easyHandles.find(eHandle);
                        siteData* siteOutput = outputIt->second;
                        // If the request did not involve an empty URL, and the request did not return empty, and the handle was not misallocated
                        if(!siteOutput->siteUrl.empty() && !siteOutput->siteContents.empty() && outputIt != easyHandles.end())
                            outputQueue->push(siteData(siteOutput->siteContents, siteOutput->siteUrl));
                    // Otherwise, print the cURL error and the associated URL
                    } else {
                        std::unordered_map<CURL*, siteData*>::iterator outputIt = easyHandles.find(eHandle);
//                        std::cout << '\r' << "CURL Error: " << message->data.result << "                                                                                                       " << std::endl << "URL: " << outputIt->second->siteUrl << "                                                          " << std::endl;
                    }
                    // If there exists a URL, repopulate the handle
                    if(urlQueue->safePop(&url))
                        updateHandleURL(eHandle, url);
                    else
                        //Otherwise, push the handle to the handlesWaitingForNewURLs queue
                        handlesWaitingForNewURLs.push(eHandle);
                    workDone = true;
                }
            }
            // Get the next message
            message = curl_multi_info_read(multiHandle, &messageQueueItems);
        }
//            std::cout << '\r' << "URLS Left: " << urlQueue -> size() <<  "Empty Transfers: " << handlesWaitingForNewURLs.size() << " Number Of Running Transfers: " << isRunning << std::endl;
        // If no work was done in last cycle, sleep to avoid tight loop
        if(!workDone)
            std::this_thread::sleep_for(sleepLockMilliseconds);
    } while(killSwitch->load() == 0);
    std::cout << "CurlThread Exiting" << std::endl;
}

/**
 * Static callback class which follows the prototype found at https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
 * This function is used by the libcurl Easy Interface curl_easy_setopt function with the CURLOPT_WRITEFUNCTION option
 * 
 * @param[in] ptr a pointer to the data delivered by cURL
 * @param[in] size "size is always 1" (https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html)
 * @param[in] nmemb the size of the data delivered by cURL
 * @param[out] buffer the siteData object output is written to
 */
size_t CurlThread::curlWriteDataCallback(char* ptr, size_t size, size_t nmemb, siteData* buffer) {
    const std::size_t maxDesiredStringSize = 100000;

    // If data of size 0 was returned, there is no work to do
    if(nmemb == 0)
        return nmemb;
    std::string str;

    // Vector used to prevent excessively massive string sizes, as extremely large strings cause inefficiencies and instability
    std::size_t charsToParse = nmemb;

    // Aims for strings of size 100000 and below.
    while(charsToParse > maxDesiredStringSize) {
        str.assign(ptr, maxDesiredStringSize);
        buffer->siteContents.push_back(str.substr(0, maxDesiredStringSize));
        ptr += maxDesiredStringSize;
        charsToParse -= maxDesiredStringSize;
    }
    // When the string is less than size 100000, the char * ptr variable is converted to a string and pushed to the buffer's string vector
    str.assign(ptr);
    buffer->siteContents.push_back(str);
    //Return nmemb, the size of the array.
    return nmemb;
};

/**
 * updateHandleURL updates the easy handle with a new URL
 * 
 * The easy handle is removed and re-added to the multi handle to update the values
 * 
 * @param eHandle the handle to refresh
 * @param url the URL the handle should query next
 */
void CurlThread::updateHandleURL(CURL* eHandle, std::string url) {
    std::unordered_map<CURL*, siteData*>::iterator it = easyHandles.find(eHandle);

    // TODO: remove these
//    std::cout << '\r' << "Query Complete: " << it->second->siteUrl << "                                                                                          " <<std::endl; 
//    std::cout << '\r' << "Querying: " << url << "                                                                                                       " << std::endl; 

    // Find the the cURL handle, update the siteData with a new URL, and clear the old site's data
    it->second->siteContents.clear();
    it->second->siteUrl = url.c_str();
    
    // Update the multi handle by removing and readding the changed handle
    curl_multi_remove_handle(multiHandle, eHandle);
    curl_easy_setopt(eHandle, CURLOPT_URL, url.c_str());
    curl_multi_add_handle(multiHandle, eHandle);
}