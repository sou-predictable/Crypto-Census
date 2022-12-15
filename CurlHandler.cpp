/**
 * @file CurlHandler.cpp
 *
 * @brief Implementation of libcurl which takes a string representation of a URL and points cURL at the specified URL for a HTTPS GET request. The output is then handled and returned as a string.
 * 
 * @author Alexander Sou
 * 
 * @version 0.1.0
 * 
 */

#include "CurlHandler.h"
#include <curl/curl.h>
#include <string>

/**
 * Points libcurl at a site and attempts to query using HTTPS. It accomplishes this by using the libcurl Easy Interface: https://curl.se/libcurl/c/libcurl-easy.html.
 * 
 * This function requires the existance of the SSL certificate "cacert.pem" within the same directory as the application. This SSL certificate can be obtained at https://curl.se/ca/cacert.pem.
 * 
 * @param url; a string representation of the URL to be queried.
 * @returns the data returned by the libcurl call in string format if cURL executed successfully, otherwise returns an empty string. 
 */
std::string CurlHandler::curlHttpsSite(std::string url) {
    CURLcode response;
    std::string curlSiteDataString;

    // Initialize cURL.
    CURL *curl = curl_easy_init();

    // Checks to see if cURL is valid and proceeds with the setup and execution if it is.
    if(curl) {
        // Sets the URL.
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        // Calls the static callback function curlCallback.
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlHandler::curlCallback);
        // Loads the SSL certificate "cacert.pem".
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
        // Passes a reference to curlSiteDataString for data outputting.
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlSiteDataString);
        // Passes a fake Chrome user agent.
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/107.0.0.0 Safari/537.36");
        // Tells CURL to follow redirects
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        // Tells CURL to only follow 3 redirects
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
        // Tells CURL to only follow HTTPS redirects
        curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS_STR, "HTTPS");
        // Tells CURL to only accept HTTP/HTTPS protocols
        curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
        // Tells CURL to only accept the first 15MBs of a webpage
        curl_easy_setopt(curl, CURLOPT_RANGE, "0-15000000");
        // Tells CURL to limit transfers to 2 minutes to avoid hanging CURL execution.
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
        // TODO: https://curl.se/libcurl/security.html

        // Executes curl.
        response = curl_easy_perform(curl);
        // Cleanup.
        curl_easy_cleanup(curl);
    }

    // If the cURL response is not CURLE_OK (success), the error code is print to stdout along with the offending URL and the curlSite returns an empty string.
    if(response != CURLE_OK) {
        // See https://curl.se/libcurl/c/libcurl-errors.html for error codes information.
        // TODO: handle "response" for CURL errors. CURL fails silently.
        return "";
    }

    return curlSiteDataString;
}

/**
 * Static callback class which follows the prototype found at https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html.
 * This function is used by the libcurl Easy Interface curl_easy_setopt function with the CURLOPT_WRITEFUNCTION option.
 * 
 * @param[in] ptr a pointer to the data delivered by cURL.
 * @param[in] size "size is always 1" (https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html).
 * @param[in] nmemb the size of the data delivered by cURL.
 * @param[out] curlCallbackDataString the data string we now use.
 */
size_t CurlHandler::curlCallback(char *ptr, size_t size, size_t nmemb, std::string *curlCallbackDataString) {
    // Cast the data returned by cURL as a static char* of size "nmemb" and append it to curlCallbackDataString. Then return nmemb, the size of the array. 
    curlCallbackDataString -> append(ptr, nmemb);
    return nmemb;
};