#include "TermMatcher.h"
#include <string>
#include <unordered_set>

/**
 * matchTerms leverages the std::string.find() function multiple times to check whether a certain number of unique terms exists within given data.
 * @param data that is checked by the term matcher
 * @param terms the set of unique terms that could potentially be matched
 * @param numRequiredTerms the number of terms which should be matched
 * @returns true if the number of unique terms matched is greater than the number of required terms; returns false otherwise
 * @returns true if terms list is empty
 */
bool TermMatcher::matchTerms(std::string data, std::unordered_set<std::string> terms, int numRequiredTerms) {
    int uniqueTermsFound = 0;
    /**
     * Handles empty term list
     * TODO: check to see if this check is nessecary, or if iterator will take care of the empty case.
     */
    if(terms.size() == 0)
        return true;
    if(data.empty())
        return false;

    std::unordered_set<std::string>::iterator i = terms.begin();

    // Enumerate through all terms
    while (i != terms.end()) {
        /** 
         * Check to see if the term exists within the data. 
         * If it does, uniqueTermsFound is incremented by 1 and checked to see if it is greater than numRequiredTerms
         */
        if(data.find(*i) != std::string::npos) {
            uniqueTermsFound++;
            if(uniqueTermsFound >= numRequiredTerms)
                return true;
        }
        i++;
    }
    return false;
}