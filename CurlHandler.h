#ifndef CURLHANDLER_H
#define CURLHANDLER_H

/**
 * @file CurlHandler.h
 *
 * @brief Implementation of libcurl which takes a string representation of a URL and points cURL at the specified URL. The output is then handled and returned as a string.
 * 
 * @author Alexander Sou
 * 
 * @version 0.1.0
 * 
 */

#include <curl/curl.h>
#include <string>
#include <iostream>

/**
 * Implementation of libcurl.
 * 
 * CurlHandler takes a string representation of a URL and points cURL at the specified URL. The output is then handled and returned as a string.
 * 
 */
class CurlHandler {
    public:
        /**
         * Queries a site with cURL using HTTPS.
         * @param url; a string representation of the URL to be queried.
         * @returns the data returned by the libcurl call in string format if cURL executed successfully, otherwise returns an empty string. 
         */
        std::string curlHttpsSite(std::string url);

    private:
        /**
         * Static callback class which follows the prototype found at https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html.
         * 
         * @param[in] ptr a pointer to the data delivered by cURL.
         * @param[in] size "size is always 1" (https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html).
         * @param[in] nmemb the size of the data delivered by cURL.
         * @param[out] curlCallbackDataString the data string we now use.
         */
        static size_t curlCallback(char *ptr, size_t size, size_t nmemb, std::string *curlCallbackDataString);
};

#endif