#ifndef TERMMATCHER_H
#define TERMMATCHER_H

#include <string>
#include <unordered_set>

/**
 * TermMatcher is used to determine if a certain number of unique terms exist within given data.
 */
class TermMatcher {
    public:
        /**
         * matchTerms leverages the std::string.find() function multiple times to check whether a certain number of unique terms exists within given data.
         * @param data that is checked by the term matcher
         * @param terms the set of unique terms that could potentially be matched
         * @param numRequiredTerms the number of terms which should be matched
         * @returns true if the number of unique terms matched is greater than the number of required terms; returns false otherwise
         * @returns true if terms list is empty
         */
        static bool matchTerms(std::string data, std::unordered_set<std::string> terms, int numRequiredTerms);
};

#endif